#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

double step = 10;      // this decides how many units do we step up down left or right.
double scale = 1.2;    // scaling factor

double win_w, win_h;

double pan_x = 0.0;   //panning along x axis  variable
double pan_y = 0.0;   //panning along y axis variable
double zoom_k = 200.0;



void draw_object(void)   // this function draws a polygon to operate with.
{
    glBegin(GL_QUADS);
   // glTexCoord2f(0.0, 0.0);
    glVertex2f(0.0, 0.0);
   // glTexCoord2f(1.0, 0.0);
    glVertex2f(1.0, 0.0);
   // glTexCoord2f(1.0, 1.0);
    glVertex2f(1.0, 1.0);
   // glTexCoord2f(.0, 1.0);
    glVertex2f(.0, 1.0);
    glEnd();
}

void zoom(double factor)  //this function decides how much to zoom
{
    zoom_k *= factor;
}

void pan(double dx, double dy)   //this function decides by factor do we pan along x and y axis. 
{
    pan_x += dx / zoom_k;
    pan_y += dy / zoom_k;
}

void zoom_at(double x, double y, double factor) // this function tells the position we reach after zooming
{
    pan(-x, -y);
    zoom(factor);
    pan(x, y);
}

void key(unsigned char key, int x, int y) //this function tells that if key is pressed then exit
{
    if (key == '\033')
        exit(0);
}

void special(int k, int x, int y)  // this function pan along the screen depending on the mouse click.
{

     if (k == GLUT_KEY_LEFT)
        pan(-step, 0);
    else if (k == GLUT_KEY_RIGHT)
        pan( step, 0);
    else if (k == GLUT_KEY_DOWN)
        pan(0, -step);
    else if (k == GLUT_KEY_UP)
        pan(0, step);

    glutPostRedisplay();
}

void mouse(int b, int s, int x, int y)//this function zooms the polygon on mouse click.
{
    if (s != GLUT_DOWN)
        return;

    y = win_h - 1 - y;

    if (b == GLUT_LEFT_BUTTON)



        zoom_at(x, y, scale);
    else if (b == GLUT_RIGHT_BUTTON)



        zoom_at(x, y, 1/scale);

    glutPostRedisplay();
}

void draw(void) //this functions draws the polygon/object on the display screen
{
    glClearColor(0.0, 0.0, 1.0, 1); //clears color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(zoom_k, zoom_k, 1);
    glTranslatef(pan_x, pan_y, 0);

    draw_object();

    glutSwapBuffers();
}

void resize(int w, int h) //this is responsible for resizing 
{
    if (h == 0)
        h = 1;

    win_w = w;
    win_h = h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
}

void display()  //this function displays the polygon on the display screen
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(0.0, 1.0, 0.0);
    draw();
}
int main(int argc,char **argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(640, 512);
    glutCreateWindow("Assignment 3 Question 1");
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutMainLoop();
    return(0);
}
