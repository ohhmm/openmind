find_library(FANN_LIBRARY_PATH
    NAMES fann libfann fann_static
    HINTS ${CMAKE_PREFIX_PATH}
)
# Prefer the release/non-debug artifact when vcpkg exposes both variants.
if(FANN_LIBRARY_PATH MATCHES "/debug/lib/")
    string(REPLACE "/debug/lib/" "/lib/" FANN_RELEASE_LIBRARY_PATH "${FANN_LIBRARY_PATH}")
    if(EXISTS "${FANN_RELEASE_LIBRARY_PATH}")
        set(FANN_LIBRARY_PATH "${FANN_RELEASE_LIBRARY_PATH}")
    endif()
endif()
find_path(FANN_INCLUDE_DIR
    NAMES fann.h
    HINTS ${CMAKE_PREFIX_PATH}
)
if(NOT FANN_INCLUDE_DIR AND FANN_LIBRARY_PATH MATCHES "/lib(fann)?$")
    # derive a sibling include directory as a fallback
    get_filename_component(_fann_lib_dir "${FANN_LIBRARY_PATH}" DIRECTORY)
    get_filename_component(_fann_lib_dir "${_fann_lib_dir}/../include" ABSOLUTE)
    if(EXISTS "${_fann_lib_dir}/fann.h")
        set(FANN_INCLUDE_DIR "${_fann_lib_dir}")
    endif()
endif()

if(FANN_LIBRARY_PATH)
    message(STATUS "FANN library: ${FANN_LIBRARY_PATH}")
    message(STATUS "FANN include: ${FANN_INCLUDE_DIR}")
    test(math) # build FANN_test without letting deps rewrite fann into a Git/dep lookup
    if(TARGET FANN_test)
        if(FANN_INCLUDE_DIR)
            target_include_directories(FANN_test PRIVATE "${FANN_INCLUDE_DIR}")
        endif()
        target_link_libraries(FANN_test PRIVATE "${FANN_LIBRARY_PATH}")
        find_package(OpenMP)
        if(OpenMP_CXX_FOUND)
            target_link_libraries(FANN_test PRIVATE OpenMP::OpenMP_CXX)
        else()
            find_library(OMP_LIBRARY NAMES omp gomp iomp5
                HINTS
                    "$ENV{HOMEBREW_PREFIX}/opt/libomp/lib"
                    "/opt/homebrew/opt/libomp/lib"
                    "/usr/local/opt/libomp/lib"
                    "$ENV{HOMEBREW_PREFIX}/lib"
                    "/opt/homebrew/lib"
                    "/usr/local/lib"
                PATHS
                    "$ENV{HOMEBREW_PREFIX}/opt/libomp/lib"
                    "/opt/homebrew/opt/libomp/lib"
                    "/usr/local/opt/libomp/lib"
            )
            if(NOT OMP_LIBRARY)
                foreach(_libomp_candidate
                    "$ENV{HOMEBREW_PREFIX}/opt/libomp/lib/libomp.dylib"
                    "/opt/homebrew/opt/libomp/lib/libomp.dylib"
                    "/usr/local/opt/libomp/lib/libomp.dylib"
                )
                    if(EXISTS "${_libomp_candidate}")
                        set(OMP_LIBRARY "${_libomp_candidate}")
                        break()
                    endif()
                endforeach()
            endif()
            if(OMP_LIBRARY)
                if(APPLE)
                    target_compile_options(FANN_test PRIVATE -Xpreprocessor -fopenmp)
                else()
                    target_compile_options(FANN_test PRIVATE -fopenmp)
                endif()
                target_link_libraries(FANN_test PRIVATE "${OMP_LIBRARY}")
            elseif(NOT WIN32)
                target_link_libraries(FANN_test PRIVATE gomp)
            endif()
        endif()
    endif()
else()
    message(WARNING "FANN library not found; skipping FANN_test")
endif()

include(FetchContent)

    FetchContent_Declare(
      NonLinearSystem
      GIT_REPOSITORY https://github.com/ohhmm/NonLinearSystem.git
      GIT_TAG main
    )
    FetchContent_MakeAvailable(NonLinearSystem)

    if(NOT WIN32)
        FetchContent_Declare(
          overheat
          GIT_REPOSITORY https://github.com/ohhmm/overheat.git
          GIT_TAG main
        )
        FetchContent_MakeAvailable(overheat)
    endif()

# Link the OpenMind runtime target needed by the third-party executables.
if(WIN32)
    set(_thirdparty_targets nonlinearsystem-src)
else()
    set(_thirdparty_targets overheat-src nonlinearsystem-src)
endif()
foreach(_thirdparty_target ${_thirdparty_targets})
    if(TARGET ${_thirdparty_target} AND TARGET rt)
        target_link_libraries(${_thirdparty_target} PRIVATE rt)
    endif()
endforeach()

# TODO: https://github.com/ohhmm/generator
