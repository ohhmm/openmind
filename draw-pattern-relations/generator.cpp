#include "gen.h"
#include "fs_io.hpp"
#include "system.hpp"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include <boost/gil.hpp>

#ifdef OPENMIND_USE_OPENGL
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <OpenCL/opencl.h>
#include <OpenCL/cl_gl_ext.h>
#include <OpenGL/CGLCurrent.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#include <CL/cl.h>
#include <CL/cl_gl.h>
#endif
#endif // OPENMIND_USE_OPENGL

namespace {
#ifdef OPENMIND_USE_OPENGL
	static const int FRAMES = 1000;
	static int gWidth = 0;
	static int gHeight = 0;
	static std::uint32_t* gPixels = nullptr;
	static GLuint gTexture = 0;
	static cl_context gContext = nullptr;
	static cl_command_queue gQueue = nullptr;
	static cl_kernel gKernel = nullptr;
	static cl_mem gImage = nullptr;
	static cl_mem gBuffer = nullptr;
	static int gFrame = 0;
	static std::chrono::high_resolution_clock::time_point gStart;
	static std::chrono::high_resolution_clock::time_point gLastFrame;
	static double gMinFps = 1e9;
	static double gMaxFps = 0.0;

	static void fatal(const char* what, cl_int err) {
		std::cerr << what << ": " << err << "\n";
		std::exit(1);
	}

	static std::string readFile(const char* path) {
		std::ifstream f(path);
		if (!f) { std::cerr << "cannot open " << path << "\n"; std::exit(1); }
		std::stringstream ss; ss << f.rdbuf(); return ss.str();
	}

	static void init() {
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
		glutInitWindowSize(gWidth, gHeight);
		glutCreateWindow("OpenCL/OpenGL Interop Pattern Relations");

		glGenTextures(1, &gTexture);
		glBindTexture(GL_TEXTURE_2D, gTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, gWidth, gHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		cl_int err;
#ifdef __APPLE__
		CGLContextObj cglContext = CGLGetCurrentContext();
		CGLShareGroupObj shareGroup = CGLGetShareGroup(cglContext);
		cl_context_properties properties[] = {
			CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
			(cl_context_properties)shareGroup, 0
		};
		gContext = clCreateContext(properties, 0, nullptr, nullptr, nullptr, &err);
#else
		gContext = clCreateContextFromType(nullptr, CL_DEVICE_TYPE_GPU, nullptr, nullptr, &err);
#endif
		if (err != CL_SUCCESS) fatal("clCreateContext", err);

		cl_device_id device;
		size_t deviceCount;
		clGetContextInfo(gContext, CL_CONTEXT_DEVICES, sizeof(device), &device, &deviceCount);
		gQueue = clCreateCommandQueue(gContext, device, 0, &err);
		if (err != CL_SUCCESS) fatal("clCreateCommandQueue", err);

		gImage = clCreateFromGLTexture(gContext, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, gTexture, &err);
		if (err != CL_SUCCESS) fatal("clCreateFromGLTexture", err);

		auto sz = static_cast<size_t>(gWidth) * static_cast<size_t>(gHeight) * sizeof(cl_uint);
		gBuffer = clCreateBuffer(gContext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sz, gPixels, &err);
		if (err != CL_SUCCESS) fatal("clCreateBuffer", err);

		const char* source = "__kernel void copy_pixels(__global const uint* src, __write_only image2d_t dst, int width, int height) {\n"
			"  int x = get_global_id(0);\n"
			"  int y = get_global_id(1);\n"
			"  if (x >= width || y >= height) return;\n"
			"  uint c = src[y * width + x];\n"
			"  float4 color = (float4)(((c >> 0) & 0xff) / 255.0f, ((c >> 8) & 0xff) / 255.0f, ((c >> 16) & 0xff) / 255.0f, ((c >> 24) & 0xff) / 255.0f);\n"
			"  write_imagef(dst, (int2)(x, y), color);\n"
			"}\n";
		size_t len = std::strlen(source);
		cl_program program = clCreateProgramWithSource(gContext, 1, &source, &len, &err);
		if (err != CL_SUCCESS) fatal("clCreateProgramWithSource", err);
		err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
		if (err != CL_SUCCESS) {
			size_t logSize;
			clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
			std::vector<char> log(logSize);
			clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, log.data(), nullptr);
			std::cerr << log.data() << "\n";
			std::exit(1);
		}
		gKernel = clCreateKernel(program, "copy_pixels", &err);
		if (err != CL_SUCCESS) fatal("clCreateKernel", err);

		err = clSetKernelArg(gKernel, 0, sizeof(gBuffer), &gBuffer);
		if (err != CL_SUCCESS) fatal("set arg 0", err);
		err = clSetKernelArg(gKernel, 1, sizeof(gImage), &gImage);
		if (err != CL_SUCCESS) fatal("set arg 1", err);
		err = clSetKernelArg(gKernel, 2, sizeof(gWidth), &gWidth);
		if (err != CL_SUCCESS) fatal("set arg 2", err);
		err = clSetKernelArg(gKernel, 3, sizeof(gHeight), &gHeight);
		if (err != CL_SUCCESS) fatal("set arg 3", err);

		gStart = std::chrono::high_resolution_clock::now();
		gLastFrame = gStart;
	}

	static void display() {
		size_t global[2] = { static_cast<size_t>((gWidth + 7) / 8 * 8), static_cast<size_t>((gHeight + 7) / 8 * 8) };
		size_t local[2] = { 8, 8 };
		if (static_cast<size_t>(gWidth) < 8) local[0] = static_cast<size_t>(gWidth);
		if (static_cast<size_t>(gHeight) < 8) local[1] = static_cast<size_t>(gHeight);
		if (local[0] == 0) local[0] = 1;
		if (local[1] == 0) local[1] = 1;

		clSetKernelArg(gKernel, 2, sizeof(gWidth), &gWidth);
		clSetKernelArg(gKernel, 3, sizeof(gHeight), &gHeight);
		clEnqueueAcquireGLObjects(gQueue, 1, &gImage, 0, nullptr, nullptr);
		clEnqueueNDRangeKernel(gQueue, gKernel, 2, nullptr, global, local, 0, nullptr, nullptr);
		clEnqueueReleaseGLObjects(gQueue, 1, &gImage, 0, nullptr, nullptr);
		clFinish(gQueue);

		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, gTexture);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 1); glVertex2f(-1, -1);
		glTexCoord2f(1, 1); glVertex2f( 1, -1);
		glTexCoord2f(1, 0); glVertex2f( 1,  1);
		glTexCoord2f(0, 0); glVertex2f(-1,  1);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glutSwapBuffers();

		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> frameDt = now - gLastFrame;
		gLastFrame = now;
		if (frameDt.count() > 0.0 && gFrame > 0) {
			double fps = 1.0 / frameDt.count();
			gMinFps = std::min(gMinFps, fps);
			gMaxFps = std::max(gMaxFps, fps);
		}

		if (++gFrame >= FRAMES) {
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> s = end - gStart;
			std::cout << "frames=" << FRAMES << "\n";
			std::cout << "average_fps=" << (FRAMES / s.count()) << "\n";
			std::cout << "min_fps=" << gMinFps << "\n";
			std::cout << "max_fps=" << gMaxFps << "\n";
			std::exit(0);
		}
		glutPostRedisplay();
	}
#endif // OPENMIND_USE_OPENGL
} // namespace

int main(int argc, char** argv)
{
	auto& programOptionValues = gen::Init(argc, argv);

	gen::PatternRelationSystem system(std::cin);
	auto img = system.GeneratePatternImage();
	if (!gen::io::fs::Save(img, programOptionValues.filepath)) {
		std::cerr << programOptionValues.filepath << " is not written\n";
	} else {
		std::cout << programOptionValues.filepath << " is written\n";
	}

#ifdef OPENMIND_USE_OPENGL
	gWidth = static_cast<int>(img.width());
	gHeight = static_cast<int>(img.height());
	if (gWidth <= 0 || gHeight <= 0) {
		return 1;
	}
	boost::gil::view(img);
	auto v = boost::gil::view(img);
	if (!v.is_1d_traversable()) {
		return 1;
	}
	gPixels = reinterpret_cast<std::uint32_t*>(v.begin().x());

	glutInit(&argc, argv);
	init();
	glutDisplayFunc(display);
	glutMainLoop();
#endif

	return 0;
}
