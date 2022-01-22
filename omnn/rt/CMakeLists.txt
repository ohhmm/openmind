lib()
if(MSVC)
	target_compile_options(${this_target} PUBLIC /constexpr:steps1000000000>)
endif()
