// OPENGL_fraction.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>

#define GL_GLEXT_PROTOTYPES
#if defined __APPLE__ || defined __linux
#include <GL/glut.h>
#else
//#include <gl/GL.h>
//#include <gl/GLU.h>
#include <glut.h>
#endif

void display();
void specialKeys();


using namespace std;


struct Position
{
    Position() : x(0), y(0) {}
    double x;
    double y;
};

Position start;
Position finish;


int XSize = 600, YSize = 600;

double xmin = -3.0, xmax = 3.0;
double ymin = -3.0, ymax = 3.0;

double xminOld = -3.0, xmaxOld = 3.0;
double yminOld = -3.0, ymaxOld = 3.0;

double xminSelect = -3.0, xmaxSelect = 3.0;
double yminSelect = -3.0, ymaxSelect = 3.0;

const int MAX_ITER_NUM = 1280;



GLfloat black[] = {0.0f, 0.0f, 0.0f}; // black color
GLfloat white[] = {1.0f, 1.0f, 1.0f}; // white color
//const int paletteSize = MAX_ITER_NUM;
//GLfloat palette[paletteSize][3];
GLfloat palette[MAX_ITER_NUM][3];




// ----------------------------------------------------------
// display() Callback function
// ----------------------------------------------------------
void display(){

 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 glOrtho(0, XSize, YSize, 0, 0, 1);
 glDisable(GL_DEPTH_TEST);
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 glTranslatef(0.375, 0.375, 0);

 glClearColor(.3, .3, .3, 0);
 glClear(GL_COLOR_BUFFER_BIT);

 double x0, y0;
 double x = 0;
 double y = 0;
 double xtemp;
 int iterNum = 0;
 GLfloat* color;
 color = (GLfloat *)malloc(3*sizeof(GLfloat));

 glBegin(GL_POINTS);
 for(int i=0; i<XSize; i++)
	 for (int j=0; j<YSize; j++)
	 {
		 x0 = xmin + (double)i/(double)XSize*(xmax - xmin);
		 y0 = ymin + (double)j/(double)YSize*(ymax - ymin);
		 // draw a line
		x = 0;
		y = 0;
		iterNum = 0;

		  while ( x*x + y*y < 4  &  iterNum < MAX_ITER_NUM )
		  {
			xtemp = x*x - y*y + x0;
			y = 2*x*y + y0;
			x = xtemp;
			iterNum ++;
		  }

		color = palette[iterNum];
		glColor3fv(color);
		glVertex2f(i, j);

	 }
	glEnd();

	//glFlush();
	glutSwapBuffers();

}

void reshape (int w, int h){ // function called when window size is changed
	//double stepX = (maxX-minX)/(GLfloat)w; // calculate new value of step along X axis
	//double stepY = (maxY-minY)/(GLfloat)h; // calculate new value of step along Y axis

	// fix the window ratio to be 1:1
	if (w>h)
		w = h;
	else
		h = w;

	XSize = w; YSize = h;

	glViewport (0, 0, (GLsizei)w, (GLsizei)h); // set new dimension of viewable screen
	glutPostRedisplay(); // repaint the window
}

// push down and drag the mouse to get the new plot region
// http://stackoverflow.com/questions/12925937/drawing-a-rectangle-on-press-and-release-of-mouse-opengl

void mouse(int button, int state, int x, int y)
{
    switch(button)
    {
    case GLUT_LEFT_BUTTON:

        if(state==GLUT_DOWN)
        {
            start.x = x; //x1
            start.y = y; //y1    
			
			xminSelect = xmin + (double)x/(XSize)*(xmax - xmin);
			yminSelect = ymin + (double)y/(YSize)*(ymax - ymin);

            cout<<"Initial pos set: (" << start.x << " " << start.y << endl;                  
        }
        if(state==GLUT_UP)
        {
            finish.x = x; //x2
            finish.y = y; //y2
            cout<<"Final pos set: (" << finish.x << " " << finish.y << endl;  

			//xmin = start.x<finish.x?start.x: finish.x;
			//xmax = start.x+finish.x - xmin;

			//ymin = start.y<finish.y?start.y: finish.y;
			//ymax = start.y + finish.y - ymin;

			xmaxSelect = xmin + (double)x/(XSize)*(xmax - xmin);
			ymaxSelect = ymin + (double)y/(YSize)*(ymax - ymin);

			// check the ratio of the width and height, reset if it is too big or too small
			double ratio = (xmaxSelect - xminSelect)/(ymaxSelect - yminSelect);
			if (ratio > 1)
			{	
				xmaxSelect = xminSelect + (ymaxSelect - yminSelect);
			}
			else if (ratio < 1)
			{
				ymaxSelect = yminSelect + (xmaxSelect - xminSelect);
			}

			// update the region
			xminOld = xmin; xmaxOld = xmax;
			yminOld = ymin; ymaxOld = ymax;

			xmin = xminSelect; xmax = xmaxSelect;
			ymin = yminSelect; ymax = ymaxSelect;

			// repaint
			glutPostRedisplay();

        }
        break;  
        //glutPostRedisplay();
    }
}

void motion( int x, int y )
{
	glBegin(GL_QUADS);
	glColor3fv(white);
	glVertex2f(start.x, start.y);
	glVertex2f(start.x, finish.y);
	glVertex2f(finish.x, finish.y);
	glVertex2f(finish.x, start.y);
	glEnd();

    finish.x = x;
    finish.y = y;

	glBegin(GL_QUADS);
	glColor3fv(white);
	glVertex2f(start.x, start.y);
	glVertex2f(start.x, finish.y);
	glVertex2f(finish.x, finish.y);
	glVertex2f(finish.x, start.y);
	glEnd();

    //glutPostRedisplay();
}



// create the palette using the method from http://wojanton.ovh.org/ang/fractals/mandelbrotCpp.php
void createPalette(){
	int N = MAX_ITER_NUM/4;
	int M = 2*N;
	for(int i=0; i < N; i++){
		palette[i][0] = (8*i)/(GLfloat)M;
		palette[i][1] = (N-4*i)/(GLfloat)M;
		palette[i][2] = (M-8*i)/(GLfloat)M;
	}
	for(int i=0; i < N; i++){
		palette[N+i][0] = (GLfloat)1;
		palette[N+i][1] = (8*i)/(GLfloat)M;
		palette[N+i][2] = (GLfloat)0;
	}
	for(int i=0; i < N; i++){
		palette[2*N+i][0] = (N-4*i)/(GLfloat)M;
		palette[2*N+i][1] = (GLfloat)1;
		palette[2*N+i][2] = (8*i)/(GLfloat)M;
	}
	for(int i=0; i < N; i++){
		palette[3*N+i][0] = (GLfloat)0;
		palette[3*N+i][1] = (M-8*i)/(GLfloat)M;
		palette[3*N+i][2] = (8*i)/(GLfloat)M;
	}
}



// ----------------------------------------------------------
// specialKeys() Callback Function
// ----------------------------------------------------------
void specialKeys( int key, int x, int y ) {

	//  Right arrow - go forward with the selection region
	if (key == GLUT_KEY_RIGHT)
	{	//rotate_y += 5;
		xmin = xminSelect; xmax = xmaxSelect;
		ymin = yminSelect; ymax = ymaxSelect;

		glutPostRedisplay();

	}
	//  Left arrow - go back to the previous region
	else if (key == GLUT_KEY_LEFT)
	{
		//rotate_y -= 5;
		xminSelect = xmin; xmaxSelect = xmax;
		yminSelect = ymin; ymaxSelect = ymax;

		xmin = xminOld; xmax = xmaxOld;
		ymin = yminOld; ymax = ymaxOld;
		
		glutPostRedisplay();

	}
	else if (key == GLUT_KEY_UP)
	{	
		//rotate_x += 5;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		//rotate_x -= 5;
	}
	//  Request display update
	//glutPostRedisplay();

}

// ----------------------------------------------------------
// main() function
// ----------------------------------------------------------
int main(int argc, char* argv[]){


//	ReadPhantom();
//	cout << phantom[0][0][0] << " " << phantom[217][125][58] << " " << phantom[217][125][59] << endl;
//	//  Initialize GLUT and process user parameters
	glutInit(&argc,argv);

	//  Request double buffered true color window with Z-buffer
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// Create window
	glutCreateWindow("Mandelbrot fraction plot");

	//  Enable Z-buffer depth test
	glEnable(GL_DEPTH_TEST);

	glutInitWindowPosition(100,100);
	glutInitWindowSize(600, 600);

	createPalette();



	// Callback functions
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(specialKeys);

	//  Pass control to GLUT for events
	glutMainLoop();

	//  Return to OS
	return 0;

}

