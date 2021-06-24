project(ChatBot)

#dep(zlib)
Find_Package(zlib)
if(NOT ZLIB_FOUND)
	ExternalProject_Add(zlib
		GIT_REPOSITORY https://github.com/madler/zlib.git
#		BUILD_IN_SOURCE 1
#		INSTALL_DIR "${CMAKE_BINARY_DIR}/zlib-prefix"
		)
	set(meta_deps zlib)
endif()

Find_Package(icu)
if(NOT ICU_FOUND)
    ExternalProject_Add(icu
        GIT_REPOSITORY https://github.com/unicode-org/icu.git
        GIT_TAG latest
        CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env
                CC=${CMAKE_C_COMPILER}
                CXX=${CMAKE_CXX_COMPILER}
                CFLAGS=${ICU_CFLAGS}
                CXXFLAGS=${ICU_CXXFLAGS}
                sh ${ICU_EP_PREFIX}/icu4c/source/configure
                --disable-shared --enable-static --disable-dyload --disable-extras
                --disable-tests --disable-samples
                --prefix=<INSTALL_DIR>
        BUILD_COMMAND make ${ICU_MAKE_EXTRA_FLAGS}
        INSTALL_COMMAND make install
        )
endif()

ExternalProject_Add(meta GIT_REPOSITORY https://github.com/meta-toolkit/meta.git)
add_dependencies(meta ${meta_deps} boost icu)


file(GLOB src *.cpp)
add_executable(chatbot ${src})
include_directories(${OpenMind_Incs})
target_link_libraries(chatbot OpenMind ${OpenMind_Libs})
add_dependencies(chatbot boost meta)
