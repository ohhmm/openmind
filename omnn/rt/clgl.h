#pragma once

#ifdef OPENMIND_USE_OPENGL

    #ifdef __APPLE__

        #define GL_SILENCE_DEPRECATION
        #include <OpenGL/gl.h>
        #include <OpenGL/glu.h>
        #include <OpenGL/glext.h>
        #include <GLUT/glut.h>

    #elif defined(MSVC)

        #include <GL/gl.h>
        #include <GL/glu.h>
        #include <GL/glext.h>
        #include <GLUT/glut.h>

    #else

        #include <GL/gl.h>
        #include <GL/glu.h>
        #include <GL/glext.h>
        #include <GL/glut.h>

    #endif

    #include <omnn/rt/cl.h>
    #include <boost/compute/interop/opengl/gl.hpp>

#endif
