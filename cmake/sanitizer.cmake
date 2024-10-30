# AddressSanitizer Configuration
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

if(ENABLE_ASAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(ASAN_FLAGS
            -fsanitize=address
            -fno-omit-frame-pointer
            -g
            -O1
        )

        # Set up suppressions file path
        set(ASAN_SUPPRESSIONS_FILE "${CMAKE_SOURCE_DIR}/cmake/leak_suppressions.txt")

        # Common ASan options for all compilers
        set(ASAN_OPTIONS_VALUE "detect_leaks=1:detect_stack_use_after_return=1:halt_on_error=1:suppressions=${ASAN_SUPPRESSIONS_FILE}")
        set(LSAN_OPTIONS_VALUE "suppressions=${ASAN_SUPPRESSIONS_FILE}")

        function(target_enable_asan target)
            target_compile_options(${target} PRIVATE ${ASAN_FLAGS})
            target_link_options(${target} PRIVATE ${ASAN_FLAGS})

            # Set environment variables for test execution
            if(TARGET ${target})
                get_target_property(target_type ${target} TYPE)
                if(target_type STREQUAL "EXECUTABLE")
                    set_tests_properties(${target} PROPERTIES
                        ENVIRONMENT
                            "ASAN_OPTIONS=${ASAN_OPTIONS_VALUE};LSAN_OPTIONS=${LSAN_OPTIONS_VALUE}"
                )
            endif()
            endif()
        endfunction()
    else()
        message(WARNING "AddressSanitizer is only supported with GCC and Clang")
        set(ENABLE_ASAN OFF CACHE BOOL "Enable AddressSanitizer" FORCE)
    endif()
endif()
