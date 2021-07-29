
option(OPENMIND_MATH_USE_LEVELDB_CACHE "Use Google LevelDB library to cache solutions" TRUE)
if(OPENMIND_MATH_USE_LEVELDB_CACHE)
    option(OPENMIND_MATH_CACHE_ALLOW_CACHE_UPGRADE "Update uptimized cached values during fetch if new optimizations applied" TRUE)
    option(OPENMIND_MATH_CACHE_VOLATILE "Clean cache each start (for debugging purposes)" ${_IS_DEBUG})
endif(OPENMIND_MATH_USE_LEVELDB_CACHE)

if(OPENMIND_MATH_USE_LEVELDB_CACHE)
	add_definitions(-DOPENMIND_MATH_USE_LEVELDB_CACHE)
	if(OPENMIND_MATH_CACHE_VOLATILE)
		add_definitions(-DOPENMIND_MATH_CACHE_VOLATILE)
	endif(OPENMIND_MATH_CACHE_VOLATILE)
	set(DEPENDENCIES ${DEPENDENCIES} google/leveldb boost)
	if(NOT WIN32)
		set(DEPENDENCIES ${DEPENDENCIES} pthread)
	endif()
    if(OPENMIND_MATH_CACHE_ALLOW_CACHE_UPGRADE)
        add_definitions(-DALLOW_CACHE_UPGRADE)
    endif()
endif()

if(OPENMIND_USE_OPENCL)
	add_definitions(-DOPENMIND_MATH_USE_OPENCL -DOPENMIND_USE_OPENCL)
	set(DEPENDENCIES ${DEPENDENCIES} ${OpenCl_Library})
endif()

if(NOT WIN32)
	set(DEPENDENCIES ${DEPENDENCIES} boost_thread boost_serialization boost_system)
endif()

lib(${DEPENDENCIES})

if (OPENMIND_USE_OPENCL)
	target_include_directories(${this_target} PUBLIC
		${OpenCL_INCLUDE_DIR}
		)
endif()

if (OPENMIND_BUILD_TESTS)
    add_subdirectory(test)
endif ()

if (OPENMIND_BUILD_3RD_PARTY_TESTS)
    add_subdirectory(3rdPartyTests)
endif ()
