cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(cuda_kernel_generator LANGUAGES CXX CUDA)

# Set the C++ and CUDA standards to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# Find the CUDA Toolkit
find_package(CUDAToolkit REQUIRED)

# Set the CUDA source files
set(CUDA_KERNEL_GENERATOR_SRC
    kernel_generator.cu
    kernel_code_generator.cpp
)

# Specify the target for the CUDA kernel generator
add_library(cuda_kernel_generator SHARED
    ${CUDA_KERNEL_GENERATOR_SRC}
)

# Set the required CUDA flags
set_target_properties(cuda_kernel_generator PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    CUDA_RUNTIME_LIBRARY Shared
)

# Specify include directories for the target
target_include_directories(cuda_kernel_generator PRIVATE
    ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}
    /home/ubuntu/openmind/omnn/math
    /usr/local/include
    /usr/include
    /usr/include/x86_64-linux-gnu
    /usr/include/c++/11
    /usr/include/c++/11/x86_64-linux-gnu
)

# Link CUDA libraries and the omnn/math library
target_link_libraries(cuda_kernel_generator
    CUDA::cudart
    omnn_math # Replace with the actual math library target name if different
)

# Ensure shared libraries are used for the linker
set_property(TARGET cuda_kernel_generator PROPERTY CUDA_RESOLVE_DEVICE_SYMBOLS ON)
