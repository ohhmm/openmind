#pragma once

#ifdef OPENMIND_USE_OPENGL

    #ifdef __APPLE__

        #define GL_SILENCE_DEPRECATION
        #include <OpenGL/gl.h>
        #include <OpenGL/glu.h>
        #include <OpenGL/glext.h>
        #if __has_include(<GLUT/glut.h>)
            #include <GLUT/glut.h>
        #endif
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wnullability-completeness-on-arrays"
        #pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
        #include <CoreGraphics/CoreGraphics.h>
        #pragma clang diagnostic pop

    #elif defined(MSVC)

        #ifdef _WIN32
            #include <Windows.h>

            #ifdef max
            #undef max
            #endif

            #ifdef min
            #undef min
            #endif
        #endif

        #include <GL/gl.h>
        #include <GL/glu.h>
        #include <GL/glext.h>
        #if __has_include(<GLUT/glut.h>)
            #include <GLUT/glut.h>
        #endif


    #else

        #include <GL/gl.h>
        #include <GL/glu.h>
        #include <GL/glx.h>
        #include <GL/glext.h>
        #if __has_include(<GL/glut.h>)
            #include <GL/glut.h>
        #endif
        #include <X11/Xlib.h>
        #include <X11/Xutil.h>

    #endif

    #include <omnn/rt/cl.h>
    #include <boost/compute/interop/opengl/context.hpp>
    #include <boost/compute/interop/opengl/gl.hpp>

#endif
