
option(OPENMIND_MATH_USE_LEVELDB_CACHE "Use Google LevelDB library to cache solutions" TRUE)
option(OPENMIND_MATH_CACHE_ALLOW_CACHE_UPGRADE "Update uptimized cached values during fetch if new optimizations applied" TRUE)

if(OPENMIND_MATH_USE_LEVELDB_CACHE)
	add_definitions(-DOPENMIND_MATH_USE_LEVELDB_CACHE)
	dep(google/leveldb)
	set(DEPENDENCIES ${DEPENDENCIES} ${leveldb_LIBRARIES})
 
    if(OPENMIND_MATH_CACHE_ALLOW_CACHE_UPGRADE)
        add_definitions(-DALLOW_CACHE_UPGRADE)
    endif()
endif()

if(OPENMIND_USE_OPENCL)
	add_definitions(-DOPENMIND_MATH_USE_OPENCL -DOPENMIND_USE_OPENCL)
	set(DEPENDENCIES ${DEPENDENCIES} ${OpenCl_Library})
endif()

if(NOT MSVC)
	set(DEPENDENCIES ${DEPENDENCIES} boost_thread boost_serialization boost_system)
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
