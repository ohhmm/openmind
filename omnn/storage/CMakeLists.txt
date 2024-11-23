option(OPENMIND_STORAGE_FOUNDATIONDB "Use Apple FoundationDB library for one of the multistorage levels" OFF)
option(OPENMIND_STORAGE_LEVELDB "Use Google LevelDB library for one of the multistorage levels" ON)

find_package(Boost REQUIRED COMPONENTS filesystem)

set(DEPENDENCIES rt Boost::filesystem)
if(OPENMIND_STORAGE_LEVELDB OR OPENMIND_STORAGE_FOUNDATIONDB)
    option(OPENMIND_STORAGE_ALLOW_UPGRADE "Update optimized cached values during fetch if new optimizations applied" ON)
    option(OPENMIND_STORAGE_VOLATILE "Clean cache each start (for debugging purposes)" OFF)

	if(OPENMIND_STORAGE_LEVELDB)
		find_package(Threads) # workaround leveldb config bug
		list(APPEND DEPENDENCIES google/leveldb)
		if(NOT WIN32)
			list(APPEND DEPENDENCIES pthread)
		endif()
	endif()

	if(OPENMIND_STORAGE_FOUNDATIONDB)
		list(APPEND DEPENDENCIES ohhmm/FoundationDB)
	endif()

endif(OPENMIND_STORAGE_LEVELDB OR OPENMIND_STORAGE_FOUNDATIONDB)

lib(${DEPENDENCIES})

if(OPENMIND_STORAGE_LEVELDB)
	target_compile_definitions(${this_target} PUBLIC OPENMIND_STORAGE_LEVELDB)
endif()
if(OPENMIND_STORAGE_FOUNDATIONDB)
	target_compile_definitions(${this_target} PUBLIC OPENMIND_STORAGE_FOUNDATIONDB)
endif()
if(OPENMIND_STORAGE_ALLOW_UPGRADE)
	target_compile_definitions(${this_target} PUBLIC ALLOW_UPGRADE OPENMIND_STORAGE_ALLOW_UPGRADE)
endif()
