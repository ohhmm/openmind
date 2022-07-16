
Find_Package(ICU)
if(NOT ICU_FOUND)
    ExternalProject_Add(icu
        GIT_REPOSITORY https://github.com/unicode-org/icu.git
        GIT_TAG release-71-1
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env
                CC=${CMAKE_C_COMPILER}
                CXX=${CMAKE_CXX_COMPILER}
                CFLAGS=${CMAKE_C_FLAGS}
                CXXFLAGS=${CMAKE_CXX_FLAGS}
                ${CMAKE_CURRENT_BINARY_DIR}/icu-prefix/src/icu/icu4c/source/configure
                --disable-shared --enable-static --disable-dyload --disable-extras
                --disable-tests --disable-samples
                --prefix=<INSTALL_DIR>
        BUILD_COMMAND make ${ICU_MAKE_EXTRA_FLAGS}
        INSTALL_COMMAND make install
        UPDATE_COMMAND ""
        )
endif()

exe(
	madler/ZLIB
	meta-toolkit/meta
    )

if(NOT ICU_FOUND)
    add_dependencies(icu)
endif()
