option(OPENMIND_MATH_USE_FOUNDATIONDB_CACHE "Use Apple FoundationDB library to distribute calculations cache" OFF)
option(OPENMIND_MATH_USE_LEVELDB_CACHE "Use Google LevelDB library to cache solutions" TRUE)

set(DEPENDENCIES ${DEPENDENCIES} rt)
if(OPENMIND_MATH_USE_LEVELDB_CACHE OR OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)
    option(OPENMIND_MATH_CACHE_ALLOW_CACHE_UPGRADE "Update optimized cached values during fetch if new optimizations applied" TRUE)
    option(OPENMIND_MATH_CACHE_VOLATILE "Clean cache each start (for debugging purposes)" OFF)

	if(OPENMIND_MATH_CACHE_VOLATILE)
		add_definitions(-DOPENMIND_MATH_CACHE_VOLATILE)
	endif(OPENMIND_MATH_CACHE_VOLATILE)

	if(OPENMIND_MATH_USE_LEVELDB_CACHE)
		add_definitions(-DOPENMIND_MATH_USE_LEVELDB_CACHE)
		set(DEPENDENCIES ${DEPENDENCIES} google/leveldb)
		if(NOT WIN32)
			set(DEPENDENCIES ${DEPENDENCIES} pthread)
		endif()
	endif()

	if(OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)
		add_definitions(-DOPENMIND_MATH_USE_FOUNDATIONDB_CACHE)
		set(DEPENDENCIES ${DEPENDENCIES} ohhmm/foundationdb)
	endif()

	if(OPENMIND_MATH_CACHE_ALLOW_CACHE_UPGRADE)
        add_definitions(-DALLOW_CACHE_UPGRADE)
    endif()

endif(OPENMIND_MATH_USE_LEVELDB_CACHE OR OPENMIND_MATH_USE_FOUNDATIONDB_CACHE)

lib(${DEPENDENCIES})
