project(ChatBot)

ExternalProject_Add(zlib GIT_REPOSITORY https://github.com/madler/zlib.git)
ExternalProject_Add(icu
    SVN_REPOSITORY http://source.icu-project.org/repos/icu/tags/latest
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env
            CC=${CMAKE_C_COMPILER}
            CXX=${CMAKE_CXX_COMPILER}
            CFLAGS=${ICU_CFLAGS}
            CXXFLAGS=${ICU_CXXFLAGS}
            sh ${ICU_EP_PREFIX}/src/ExternalICU/source/configure
            --disable-shared --enable-static --disable-dyload --disable-extras
            --disable-tests --disable-samples
            --prefix=<INSTALL_DIR>
    BUILD_COMMAND make ${ICU_MAKE_EXTRA_FLAGS}
    INSTALL_COMMAND make install
    )
ExternalProject_Add(meta GIT_REPOSITORY https://github.com/meta-toolkit/meta.git)
add_dependencies(meta boost zlib icu)


file(GLOB src *.cpp)
add_executable(chatbot ${src})
include_directories(${OpenMind_Incs})
target_link_libraries(chatbot OpenMind ${OpenMind_Libs})
add_dependencies(chatbot boost meta)