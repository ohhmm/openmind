option(OPENMIND_MATH_USE_FOUNDATIONDB_CACHE "Use Apple FoundationDB library to distribute calculations cache" ON)
option(OPENMIND_MATH_USE_LEVELDB_CACHE "Use Google LevelDB library to cache solutions" TRUE)
if(OPENMIND_MATH_USE_LEVELDB_CACHE OR OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)
    option(OPENMIND_MATH_CACHE_ALLOW_CACHE_UPGRADE "Update optimized cached values during fetch if new optimizations applied" TRUE)
    option(OPENMIND_MATH_CACHE_VOLATILE "Clean cache each start (for debugging purposes)" ${_IS_DEBUG})

	if(OPENMIND_MATH_CACHE_VOLATILE)
		add_definitions(-DOPENMIND_MATH_CACHE_VOLATILE)
	endif(OPENMIND_MATH_CACHE_VOLATILE)

	if(OPENMIND_MATH_USE_LEVELDB_CACHE )
		add_definitions(-DOPENMIND_MATH_USE_LEVELDB_CACHE)
		set(DEPENDENCIES ${DEPENDENCIES} google/leveldb boost)
		if(NOT WIN32)
			set(DEPENDENCIES ${DEPENDENCIES} pthread)
		endif()
	endif()

	if(OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)
		add_definitions(-DOPENMIND_MATH_USE_FOUNDATIONDB_CACHE)
		set(DEPENDENCIES ${DEPENDENCIES} apple/foundationdb boost)
	endif()

	if(OPENMIND_MATH_CACHE_ALLOW_CACHE_UPGRADE)
        add_definitions(-DALLOW_CACHE_UPGRADE)
    endif()

endif(OPENMIND_MATH_USE_LEVELDB_CACHE OR OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)

if(OPENMIND_USE_OPENCL)
	add_definitions(-DOPENMIND_MATH_USE_OPENCL -DOPENMIND_USE_OPENCL)
	set(DEPENDENCIES ${DEPENDENCIES} OpenCL)
endif()

if(NOT WIN32)
	set(DEPENDENCIES ${DEPENDENCIES} boost_thread boost_serialization boost_system)
endif()

set(DEPENDENCIES ${DEPENDENCIES} rt)
    
lib(${DEPENDENCIES})

if (OPENMIND_BUILD_TESTS)
    add_subdirectory(test)
endif ()

if (OPENMIND_BUILD_3RD_PARTY_TESTS)
    add_subdirectory(3rdPartyTests)
endif ()
