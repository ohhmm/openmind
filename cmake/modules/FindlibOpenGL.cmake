# Find OpenGL
include(FindPackageHandleStandardArgs)

if(APPLE)
    find_package(OpenGL REQUIRED)
    set(libOpenGL_FOUND ${OPENGL_FOUND})
    set(libOpenGL_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
    set(libOpenGL_LIBRARIES ${OPENGL_LIBRARIES})
else()
    find_path(libOpenGL_INCLUDE_DIR
        NAMES GL/gl.h
        PATHS
            /usr/include
            /usr/local/include
    )

    find_library(libOpenGL_LIBRARY
        NAMES GL OpenGL
        PATHS
            /usr/lib
            /usr/local/lib
    )

    find_package_handle_standard_args(libOpenGL
        REQUIRED_VARS libOpenGL_LIBRARY libOpenGL_INCLUDE_DIR
    )

    if(libOpenGL_FOUND)
        set(libOpenGL_INCLUDE_DIRS ${libOpenGL_INCLUDE_DIR})
        set(libOpenGL_LIBRARIES ${libOpenGL_LIBRARY})
    endif()
endif()
