set(deps
    libgen
    OpenGL
)

if(OPENMIND_USE_OPENGL)
    if(WIN32)
        set(deps ${deps} opengl32 gdi32 user32)
    elseif(APPLE)
        set(deps ${deps} CoreGraphics)
    else()
        find_package(X11 REQUIRED)
        set(deps ${deps} GL GLU ${X11_LIBRARIES})
    endif()
endif()

exe(${deps})
