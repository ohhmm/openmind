lib(boost_serialization)

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
