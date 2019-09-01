
option(OPENMIND_MATH_USE_LEVELDB_CACHE "Use Google LevelDB library to cache solutions" FALSE)

set(DEPENDENCIES "")
if(OPENMIND_MATH_USE_LEVELDB_CACHE)
	add_definitions(-DOPENMIND_MATH_USE_LEVELDB_CACHE)
	dep(google/leveldb)
	set(DEPENDENCIES "${DEPENDENCIES} leveldb")
endif()

lib(${DEPENDENCIES})

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${OpenCL_INCLUDE_DIR}
	)

if (${OPENMIND_BUILD_TESTS})
    add_subdirectory(test)
endif ()

if (${OPENMIND_BUILD_3RD_PARTY_TESTS})
    add_subdirectory(3rdPartyTests)
endif ()
