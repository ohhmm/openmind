
#define CL_TARGET_OPENCL_VERSION 200
#include <omnn/math/Variable.h>
#include <omnn/rt/cl.h>
using namespace omnn::math;

#include <boost/archive/binary_oarchive.hpp>

#ifdef OPENMIND_USE_OPENCL
#include <boost/compute.hpp>
#include <boost/compute/core.hpp>
#endif

#include <boost/filesystem.hpp>

#include <boost/gil.hpp>

#include <boost/lambda2.hpp>

#include <boost/mpl/vector.hpp>

#include <boost/multiprecision/cpp_int.hpp>

#include <boost/program_options.hpp>

#include <fstream>

#include <boost/serialization/vector.hpp>

#include "gen.h"


namespace {

	DECL_VA(i);
	DECL_VA(n);
	DECL_VA(was);
	DECL_VA(Result);
	

	Valuable::va_names_t Names = {
		{"i", i},
		{"N", n},
		{"n", n},
		{"was", was},
		{"Result", was}
	};

    auto OptionsPtr = std::make_shared<boost::program_options::options_description>("Options");
	boost::program_options::options_description& Options = *OptionsPtr;
	gen::options optionValues;
	auto& desc = Options.add_options()
		("help,h", "Produce help message")
		("file,f,o", boost::program_options::value(&optionValues.filepath)->default_value("wow.png"), "Output file path")
		("background-color,background-colour,background,b,bc,bg,bgc", boost::program_options::value(&optionValues.color)->default_value(0), "Image background color")
		("blend-file,blend,bf", boost::program_options::value(&optionValues.blend_files), "Blend images")
		;
	boost::program_options::variables_map vm;

}

namespace gen {

	std::shared_ptr<boost::program_options::options_description> Optr(){
		return OptionsPtr;
	}

	boost::program_options::options_description_easy_init& ProgramOptionsDescription() {
		return desc;
	}

    boost::program_options::options_description_easy_init& AddCommandLineProgramOption() {
        return desc;
    }

	const options& Init(int argc, char** argv)
	{
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, Options), vm);
		boost::program_options::notify(vm);
		if (vm.count("help")) {
			std::cout << "Usage: options_description [options]\n" << Options;
			exit(0);
		}
		return optionValues;
	}

	namespace {
	}
	void ProcessOptions() {
		using namespace ::boost::gil;
		typedef boost::mpl::vector< gray8_image_t
			, gray16_image_t
			, rgb8_image_t
			, rgba8_image_t
		> my_img_types;
		//any_image<my_img_types> runtime_image;
		//packed_pixel_dynamic_image img;
		//for (auto& blendFilePath : optionValues.blend_files) {
		//	png_read_and_convert_image(blendFilePath, runtime_image);
		//}
	}

	const Valuable::va_names_t& InitialVarNames() {
		return Names;
	}

	Valuable BuildFormula(const std::string& s, const Valuable::va_names_t& varNames) {
		auto names = varNames;
		names["i"] = i;
        names["n"] = n;
		names["was"] = was;
		names["Result"] = was;
		Valuable pattern(s, names, true);
		return pattern;
	}

	Valuable BuildFormula(const std::string& s, const Valuable::vars_cont_t& knownValues, const Valuable::va_names_t& varNames) {
		auto formula = BuildFormula(s, varNames);
		if (formula.eval(knownValues)) {
			//formula.optimize();
		}
		return formula;
	}

	Valuable BuildFormula(const std::string& s, const Valuable& nValue, const Valuable::va_names_t& varNames) {
		auto formula = BuildFormula(s, varNames);
		formula.Eval(n, nValue);
		return formula;
	}


	bool Generate(const Valuable& pattern, float* data, size_t wgsz) {
#ifdef OPENMIND_USE_OPENCL
		// build OpenCL kernel
		auto& ComputeUnitsWinner = omnn::rt::GetComputeUnitsWinnerDevice();
		boost::compute::context context(ComputeUnitsWinner);
		boost::compute::command_queue queue(context, ComputeUnitsWinner);
		auto code = pattern.OpenCL();
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
		std::cout << pattern << " >>> OpenCl >>> " << code << std::endl;
#endif // !NOOMDEBUG
		boost::compute::kernel k(boost::compute::program::build_with_source(code, context), "f");

		auto sz = wgsz * sizeof(cl_float);
		boost::compute::buffer ctx(context, sz,
			boost::compute::memory_object::read_write | boost::compute::memory_object::use_host_ptr,
			data);
		k.set_arg(0, ctx);

		// run the add kernel
		auto ready = queue.enqueue_1d_range_kernel(k, 0, wgsz, 0);
		queue.finish();
		// transfer results to the host array 'c'
		queue.enqueue_read_buffer(ctx, 0, sz, data, { ready });
		queue.finish();

		return true;
#else
		return false;
#endif
	}

	bool Generate(const Valuable& pattern, uint32_t* data, size_t wgsz) {
#ifdef OPENMIND_USE_OPENCL
		// build OpenCL kernel
		auto& ComputeUnitsWinner = omnn::rt::GetComputeUnitsWinnerDevice();
		boost::compute::context context(ComputeUnitsWinner);
		boost::compute::command_queue queue(context, ComputeUnitsWinner);
		auto code = pattern.OpenCLuint();
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
		std::cout << pattern << " >>> OpenCl >>> " << code << std::endl;
#endif // !NOOMDEBUG
		boost::compute::kernel k(boost::compute::program::build_with_source(code, context), "f");

		auto sz = wgsz * sizeof(cl_uint);
		boost::compute::buffer ctx(context, sz,
			boost::compute::memory_object::read_write | boost::compute::memory_object::use_host_ptr,
			data);
		k.set_arg(0, ctx);

		// run the add kernel
		auto ready = queue.enqueue_1d_range_kernel(k, 0, wgsz, 0);
		queue.finish();
		// transfer results to the host array 'c'
		queue.enqueue_read_buffer(ctx, 0, sz, data, { ready });
		queue.finish();

		return true;
#else
		return false;
#endif
	}

	Valuable BuildFormulaAGRBofWidthHeightXYiN(uint32_t w, uint32_t h, Valuable r, Valuable g, Valuable b)
	{
		auto N = w * h;
		Valuable::vars_cont_t vars = {
			{"w"_va, w},
			{"h"_va, h},
			{"x"_va, i % w},
			{"y"_va, (i - (i % w)) / w},
			{n, N},
			{"N"_va, N},
			{Result, was},
		};
		{
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
			Valuable::OptimizeOff oo;
#endif
			r.eval(vars);
			g.eval(vars);
			b.eval(vars);
		}
		constexpr auto alpha = 0xff;

		// FIXME: slow masks
		//auto f =  ((b.And(8, 0xff) + (alpha << 8)).shl(8) + g.And(8, 0xff)).shl(8) + r.And(8, 0xff);
		auto f = (((b % 256) + (alpha << 8)).shl(8) + (g % 256)).shl(8) + (r % 256);
		auto s = f.str();
		#ifndef NOOMDEBUG
		std::cout << " R:" << r << " G:" << g << " B:" << b << std::endl;
		if(f.IsInt())
			std::cout << std::hex << f.ca();
		else
			std::cout << s;
		std::cout << std::endl;
		#endif
		return gen::BuildFormula(s, N);
	}

	boost::gil::rgba8_image_t GeneratePatternImage(uint32_t width, uint32_t height, Valuable red, Valuable green, Valuable blue)
	{
		auto y = gen::BuildFormulaAGRBofWidthHeightXYiN(width, height, red, green, blue);
		boost::gil::rgba8_image_t img(width, height);
		auto v = view(img);
		#if 0
		auto i = 0;
		for (auto& px : v) {
			boost::gil::get_color(px, boost::gil::red_t())= i++;
		}
		#endif
		assert(v.is_1d_traversable());
		auto p = v.begin().x();
		gen::Generate(y, (uint32_t*)p, width * height);
		return img;
	}

} // namespace gen
