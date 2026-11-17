set(deps
    libgen
    OpenGL
)

if(OPENMIND_USE_OPENGL)
    if(WIN32)
        set(deps ${deps} freeglut)
    elseif(APPLE)
        set(deps ${deps} GLUT)
    else()
        set(deps ${deps} GLUT GL GLU)
    endif()
endif()

exe(${deps})
