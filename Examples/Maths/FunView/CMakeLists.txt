find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(
        ${OPENGL_INCLUDE_DIRS}
        ${GLUT_INCLUDE_DIRS}
    )
exe(math ${OPENGL_gl_LIBRARY} ${OPENGL_glu_library})
