#include <windows.h>
#include<gl/glut.h>
#include<gl/freeglut_ext.h>
#include<gl/freeglut_std.h>
#include<gl/freeglut.h>
#include<gl/GLU.h>
#include<gl/GL.h>
#include<stdio.h>
#include<stdlib.h>

void init()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
}

void myMouseMotion(int x, int y) {
	printf("x = %d, y = %d\n", x, y);
	// left
	if ((x > 0 || x == 0) && (x < 250 || x == 250)) {
		glRotatef(45, 0, 0, 1);
	}

	// right
	if ((x > 250 || x == 250) && (x < 500 || x == 500)) {
		glRotatef(-45, 0, 0, 1);
	}
}

void myMouse(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
			glRotated(45, 0, 0, 1);
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
			glRotated(-45, 0, 0, 1);
		break;
	default:
		break;
	}
}

void MyDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(60, 1, 1, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0, 0, -35);
	glPushMatrix();
	glTranslatef(-10, 5, 0);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glColor3ub(255, 0, 0);
	glutSolidCube(10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(10, 5, 0);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glColor3ub(0, 0, 255);
	glutSolidCube(11);
	glPopMatrix();

	glutSwapBuffers();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("20011689 ¹Ú¹Î¼±");
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluLookAt(1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	init();
	glutDisplayFunc(MyDisplay);
	glutMotionFunc(myMouseMotion);
	glutMouseFunc(myMouse);
	glutMainLoop();

	return 0;
}
