#pragma once

#ifdef OPENMIND_USE_OPENGL

    #ifdef __APPLE__

        #define GL_SILENCE_DEPRECATION
        #include <OpenGL/gl.h>
        #include <OpenGL/glu.h>
        #include <OpenGL/glext.h>
        #if __has_include(<GLUT/glut.h>)
            #include <GLUT/glut.h>
        #endi

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
        #endi
    #else

        #include <GL/gl.h>
        #include <GL/glu.h>
        #include <GL/glext.h>
        #if __has_include(<GL/glut.h>)
            #include <GL/glut.h>
        #endi

    #endif

    #include <omnn/rt/cl.h>
    #include <boost/compute/interop/opengl/gl.hpp>

#endif
