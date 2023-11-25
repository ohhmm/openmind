option(OPENMIND_STORAGE_FOUNDATIONDB "Use Apple FoundationDB library for one of the multistorage levels" OFF)
option(OPENMIND_STORAGE_LEVELDB "Use Google LevelDB library for one of the multistorage levels" TRUE)

set(DEPENDENCIES ${DEPENDENCIES} rt)
if(OPENMIND_STORAGE_LEVELDB OR OPENMIND_STORAGE_FOUNDATIONDB)
    option(OPENMIND_STORAGE_ALLOW_UPGRADE "Update optimized cached values during fetch if new optimizations applied" TRUE)
    option(OPENMIND_STORAGE_VOLATILE "Clean cache each start (for debugging purposes)" OFF)


	if(OPENMIND_STORAGE_LEVELDB)
		find_package(Threads) # workaround leveldb config bug
		set(DEPENDENCIES ${DEPENDENCIES} google/leveldb)
		if(NOT WIN32)
			set(DEPENDENCIES ${DEPENDENCIES} pthread)
		endif()
	endif()

	if(OPENMIND_STORAGE_FOUNDATIONDB)
		add_definitions(-DOPENMIND_STORAGE_FOUNDATIONDB)
		set(DEPENDENCIES ${DEPENDENCIES} ohhmm/foundationdb)
	endif()

	if(OPENMIND_STORAGE_ALLOW_UPGRADE)
        add_definitions(-DALLOW_UPGRADE)
    endif()

endif(OPENMIND_STORAGE_LEVELDB OR OPENMIND_STORAGE_FOUNDATIONDB)

lib(${DEPENDENCIES})

if(OPENMIND_STORAGE_LEVELDB)
	target_compile_definitions(${this_target} PUBLIC OPENMIND_STORAGE_LEVELDB)
endif()
