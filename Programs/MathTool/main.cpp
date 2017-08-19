//
// Created by Сергей Кривонос on 16.08.17.
//

#include <GL/gl.h>
#include <GL/glu.h>
#include <GLUT/GLUT.h>
#include <vector>

//int main()
//{
//    auto hwnd = glutCreateWindow("MathTool");
//    //Frustum(-1, 1, -1, 1, 1, -1);
//    return 0;
//}

GLfloat mat_red_diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
GLfloat mat_green_diffuse[] = { 0.0, 0.7, 0.1, .5 };
GLfloat mat_blue_diffuse[] = { 0.0, 0.1, 0.7, 1.0 };
GLfloat mat_yellow_diffuse[] = { 0.7, 0.8, 0.1, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 100.0 };
GLfloat knots[8] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0 };
const int N = 16;
GLfloat pts1[4][4][3], pts2[4][4][3];
GLfloat pts3[4][4][3], pts4[4][4][3];



static void
display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCallList(1);
    glFlush();
}

int
main(int argc, char **argv)
{
    
    
    glutInit(&argc, argv);
    glutCreateWindow("x*y");
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    GLUnurbsObj * nurb = gluNewNurbsRenderer();
    gluNurbsProperty(nurb, GLU_SAMPLING_TOLERANCE, 2.0);
    gluNurbsProperty(nurb, GLU_DISPLAY_MODE, GLU_POINT);
    
    /* Build control points for NURBS mole hills. */
    int u, v;
    for(u=0; u<4; u++) {
        for(v=0; v<4; v++) {
            GLfloat x = u;
            GLfloat y = v;
            
            /* Red. */
            pts1[u][v][0] = 2.0*((GLfloat)x);
            pts1[u][v][1] = 2.0*((GLfloat)y);
            pts1[u][v][2] = x*y;
            
            /* Green. */
            pts2[u][v][0] = 2.0*((GLfloat)x - 3.);
            pts2[u][v][1] = 2.0*((GLfloat)y - 3.0);
            pts2[3-u][3-v][2] = (-x*y);

            
            /* Blue. */
            pts3[u][v][0] = 2.0*((GLfloat)x - 3.0);
            pts3[u][v][1] = 2.0*((GLfloat)y);
            pts3[3-u][v][2] = -x*y;

            
            /* Yellow. */
            pts4[u][v][0] = 2.0*((GLfloat)x);
            pts4[u][v][1] = 2.0*((GLfloat)y - 3.0);
            pts4[u][3-v][2] = x*(-y);
        }
    }
//    /* Stretch up red's far right corner. */
//    pts1[3][3][2] = 6;
//    /* Pull down green's near left corner a little. */
//    pts2[0][0][2] = -2;
//    /* Turn up meeting of four corners. */
//    pts1[0][0][2] = 1;
//    pts2[3][3][2] = 1;
//    pts3[3][0][2] = 1;
//    pts4[0][3][2] = 1;
    
    glMatrixMode(GL_PROJECTION);
    
    gluPerspective(105.0, 1.0, 2.0, 24.0);
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(0.0, 0.0, -15.0);
    glRotatef(330.0, 1.0, 0.0, 0.0);
    
    glNewList(1, GL_COMPILE);
    /* Render red hill. */
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_red_diffuse);
    gluBeginSurface(nurb);
    gluNurbsSurface(nurb, 8, knots, 8, knots,
                    4 * 3, 3, &pts1[0][0][0],
                    4, 4, GL_MAP2_VERTEX_3);
    gluEndSurface(nurb);
    
    /* Render green hill. */
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_green_diffuse);
    gluBeginSurface(nurb);
    gluNurbsSurface(nurb, 8, knots, 8, knots,
                    4 * 3, 3, &pts2[0][0][0],
                    4, 4, GL_MAP2_VERTEX_3);
    gluEndSurface(nurb);
    
    /* Render blue hill. */
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_blue_diffuse);
    gluBeginSurface(nurb);
    gluNurbsSurface(nurb, 8, knots, 8, knots,
                    4 * 3, 3, &pts3[0][0][0],
                    4, 4, GL_MAP2_VERTEX_3);
    gluEndSurface(nurb);
    
    /* Render yellow hill. */
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_yellow_diffuse);
    gluBeginSurface(nurb);
    gluNurbsSurface(nurb, 8, knots, 8, knots,
                    4 * 3, 3, &pts4[0][0][0],
                    4, 4, GL_MAP2_VERTEX_3);
    gluEndSurface(nurb);
    glEndList();
    
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;             /* ANSI C requires main to return int. */
}
