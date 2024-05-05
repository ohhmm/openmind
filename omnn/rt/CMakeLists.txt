set(DEPENDENCIES boost)
if(Boost_FOUND)
    if(NOT MSVC OR OPENMIND_USE_CONAN)
        set(DEPENDENCIES ${DEPENDENCIES} Boost::filesystem)
        if(OPENMIND_BUILD_TESTS)
            set(DEPENDENCIES ${DEPENDENCIES}
                Boost::test_exec_monitor
                Boost::unit_test_framework
                )
        endif()
    endif()
endif()

if(OPENMIND_USE_OPENCL)
    set(DEPENDENCIES ${DEPENDENCIES} OpenCL)
endif()

# Define the source files for the rt target
set(RT_SOURCES
    Prime.cpp
    Neuron.cpp
    GC.cpp
    cl.cpp
    tasq.cpp
)

# Define the header files for the rt target
set(RT_HEADERS
    Prime.h
    Neuron.h
    GC.h
    cl.h
    tasq.h
    AsyncNeuron.h
    diag.hpp
    each.hpp
    fastest.hpp
    clgl.h
)

# Add the rt library target with the specified source files
add_library(rt STATIC ${RT_SOURCES} ${RT_HEADERS})

# Add preprocessor definition to use Boost's prime number functionality
add_definitions(-DOPENMIND_PRIME_TABLE_BOOST)

# Define _rt_SRC_DIR to point to the current source directory
target_compile_definitions(rt PRIVATE _rt_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

# Link the Boost and OpenCL libraries with the rt target
if(TARGET rt)
    target_link_libraries(rt PUBLIC Boost::filesystem Boost::system Boost::thread OpenCL)
endif()

# Diagnostic message to confirm source and header files
message("Diagnostic: RT_SOURCES = ${RT_SOURCES}")
message("Diagnostic: RT_HEADERS = ${RT_HEADERS}")
