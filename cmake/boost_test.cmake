# Configure Boost test targets with consistent settings
function(test)
    if(NOT OPENMIND_BUILD_TESTS)
        return()
    endif()

    # Get both the parent and test directory names to create a unique target name
    get_filename_component(parent_dir ${CMAKE_CURRENT_SOURCE_DIR}/.. NAME)
    get_filename_component(test_dir ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    get_filename_component(full_path ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)

    # Create a unique identifier based on the full path hash
    string(MD5 path_hash "${full_path}")
    string(SUBSTRING "${path_hash}" 0 8 short_hash)

    # Ensure parent_dir is not empty to avoid naming conflicts
    if(NOT parent_dir OR parent_dir STREQUAL "")
        message(WARNING "Parent directory name is empty, using path hash for uniqueness")
        set(parent_dir "test_${short_hash}")
    endif()

    # Find all test source files
    file(GLOB TEST_SOURCES *.cpp)

    # Create separate targets for each test file
    foreach(test_source ${TEST_SOURCES})
        get_filename_component(test_name_base ${test_source} NAME_WE)

        # Create unique target name using sanitized directory names, test name, and hash
        string(REPLACE " " "_" parent_dir_clean "${parent_dir}")
        string(REPLACE " " "_" test_dir_clean "${test_dir}")
        string(REPLACE " " "_" test_name_clean "${test_name_base}")
        set(target_name "${parent_dir_clean}_${test_dir_clean}_${test_name_clean}_${short_hash}")
        set(test_name "${parent_dir_clean}_${test_dir_clean}_${test_name_clean}_test")

        add_executable(${target_name} ${test_source})

        if(MSVC)
            # Ensure consistent iterator debug level with Boost
            add_definitions(-D_ITERATOR_DEBUG_LEVEL=2)
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_ITERATOR_DEBUG_LEVEL=2")
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /D_ITERATOR_DEBUG_LEVEL=2")

            target_compile_definitions(${target_name} PRIVATE
                _ITERATOR_DEBUG_LEVEL=2
                _DEBUG_FUNCTIONAL_MACHINERY
            )

            # Propagate iterator debug level to dependencies
            get_target_property(test_deps ${target_name} LINK_LIBRARIES)
            if(test_deps)
                foreach(dep ${test_deps})
                    if(TARGET ${dep})
                        target_compile_definitions(${dep} PRIVATE
                            _ITERATOR_DEBUG_LEVEL=2
                            _DEBUG_FUNCTIONAL_MACHINERY
                        )
                    endif()
                endforeach()
            endif()
        endif()

        # Set CMP0167 policy to NEW to address FindBoost warning
        if(POLICY CMP0167)
            cmake_policy(SET CMP0167 NEW)
        endif()

        # Define standard Boost search paths
        set(BOOST_PATHS_HINTS
            C:/Boost
            /usr/local/lib/cmake/Boost-*
            /usr/lib/x86_64-linux-gnu/cmake/Boost-*
            /usr/local/share/boost-*
        )

        # Use the same Boost configuration as main CMakeLists.txt
        if(OPENMIND_USE_VCPKG)
            find_package(Boost CONFIG COMPONENTS unit_test_framework REQUIRED)
        else()
            find_package(Boost ${OPENMIND_REQUIRED_BOOST_VERSION}
                CONFIG
                COMPONENTS unit_test_framework
                HINTS ${BOOST_PATHS_HINTS}
                REQUIRED)
        endif()

        target_link_libraries(${target_name} PRIVATE
            ${this_target}
            ${BOOST_TEST_LINK_LIBS}
        )

        add_test(NAME ${test_name} COMMAND ${target_name})
        set_tests_properties(${test_name} PROPERTIES TIMEOUT 300)
    endforeach()
endfunction()
