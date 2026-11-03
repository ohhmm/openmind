# Internship Onboarding: Vulkan, OpenCL, and OpenCL/OpenGL Interop FPS Examples

This page records the path from an OpenMind fork to three standalone graphics examples, with the agent requests used and measured results.

## Prerequisites

- macOS with Xcode Command Line Tools
- CMake 3.16+ and a C++17 compiler
- For Vulkan: LunarG Vulkan SDK and `glslc` on `PATH`
- For OpenCL/OpenCL-OpenGL interop: Apple/OpenCL frameworks and OpenGL/GLUT frameworks
- Git and (optionally) Ollama for `./do.sh`

## 1. Fork and Clone

```sh
git clone https://github.com/<your-account>/openmind.git
cd openmind
git remote add upstream https://github.com/ohhmm/openmind.git
git fetch upstream
```

## 2. How to Invoke the Agent

Use `./do.sh` for a one-shot, non-persistent request:

```sh
./do.sh "create a self-sufficient new directory with a project that draws using Vulkan a sphere with shade"
```

`./do.sh` installs/ensures Ollama when needed, selects `beam:cloud` or falls back to `qwen3.5:9b`, and launches Claude with the prompt directly.

Use `./start.sh` when the task should be persisted and resumed through `task.md`:

```sh
./start.sh "create a self-sufficient new directory with a project that draws using Vulkan a sphere with shade"
```

`./start.sh` writes the request to `task.md`, copies it to `thinking.log`, and then runs `act.sh`.

Difference:

| Entry point | Task input | State | Best use |
|---|---|---|---|
| `./do.sh` | Direct command-line argument or `Hi` | No saved task file by itself | Fast one-off request |
| `./start.sh` | Argument becomes `task.md`; otherwise reads existing `task.md` | Persists `task.md` and `thinking.log` | Resumable/multi-step internship task |

## 3. Requests Made

The following requests produced the resulting standalone examples:

1. `/pre-push` — run the consolidated pre-push QA checklist and report push readiness.
2. `while we waiting for CI status, could you please, be kind to create a selfsufficient new directory with project that draws using vulkan a sphere with shade`
3. `And yet another one that draws with opencl directly into videobuffer to compare FPS`
4. `there is opencl/opengl interop in opencl standard` then `yes`
5. `ok, this is super, lets print to console min, max, avarage FPS on exit`
6. `compare with vulkan`
7. `lets internship make onbording plan to the wiki about this`

## 4. Created Standalone Projects

Place each generated project in a standalone subfolder outside the main OpenMind build, for example:

```text
standalone/
  vulkan-sphere/
  opencl-video-buffer/
  opencl-opengl-interop/
```

Expected contents:

- `vulkan-sphere`: GLFW-free CMake project using Vulkan, shader compilation with `glslc`, indexed sphere mesh, depth buffer, and lighting.
- `opencl-video-buffer`: OpenCL kernel writing directly into a 1920x1080 buffer; writes `opencl_video_buffer_last.ppm` and reports FPS.
- `opencl-opengl-interop`: OpenCL writes directly into an OpenGL texture via `clCreateFromGLTexture`; displays the result with GLUT and reports min/max/average FPS.

Build commands after copying a project:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/vulkan_sphere
```

For the OpenCL benchmark:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/opencl_video_buffer
```

For the OpenCL/OpenGL interop benchmark:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/opencl_opengl_interop
```

## 5. Measured Results

- OpenCL → in-memory video buffer: about `1096` FPS.
- OpenCL → OpenGL interop → display: about `124` average FPS, min `60.5`, max `1295`.
- Vulkan sphere: instrumentation added for min/max/average FPS on exit; no local run was possible in this environment because no Vulkan SDK was installed.

## 6. Bring the Result Back

1. Keep standalone graphics demos outside the OpenMind CMake targets so they do not affect OpenMind CI.
2. Copy the validated project into your fork only after it builds and runs.
3. Record changes and onboarding steps in the wiki.
4. Keep each repository-facing change to one commit per pull request.
