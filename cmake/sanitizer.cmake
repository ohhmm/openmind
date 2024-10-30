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

        function(target_enable_asan target)
            target_compile_options(${target} PRIVATE ${ASAN_FLAGS})
            target_link_options(${target} PRIVATE ${ASAN_FLAGS})

            if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
                target_compile_definitions(${target} PRIVATE
                    ASAN_OPTIONS="detect_leaks=1:detect_stack_use_after_return=1"
                )
            endif()
        endfunction()
    else()
        message(WARNING "AddressSanitizer is only supported with GCC and Clang")
        set(ENABLE_ASAN OFF CACHE BOOL "Enable AddressSanitizer" FORCE)
    endif()
endif()
