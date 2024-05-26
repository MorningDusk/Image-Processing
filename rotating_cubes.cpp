#include <windows.h>
#include<gl/glut.h>
#include<gl/freeglut_ext.h>
#include<gl/freeglut_std.h>
#include<gl/freeglut.h>
#include<gl/GLU.h>
#include<gl/GL.h>
#include<stdio.h>
#include<stdlib.h>

void init() {

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float x = 0.3, y = 0.6;
	gluPerspective(40, 0.1, 500, 500);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
}

void mouseStatus(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (x <= 250)
			glRotatef(10, 0.0, 0.0, -1.0);
		else
			glRotatef(10, 0.0, 0.0, 1.0);
	}

	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		float mouseCurPositionX = (float)(x - (float)500 / 2) * (float)(1.0 / (float)(500 / 2.0));
		float mouseCurPositionY = -(float)(y - (float)500 / 2) * (float)(1.0 / (float)(500 / 2.0));
		glTranslatef(mouseCurPositionX, mouseCurPositionY, 0.0);
	}
	glutPostRedisplay();

}

void mydisplay() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(1.0, 0.0, 0.0);
	glPushMatrix();
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glutSolidCube(0.2);
	glPopMatrix();

	glColor3f(0.0, 0.0, 1.0);
	glPushMatrix();
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glTranslatef(0.5, 0.3, 0.2);
	glutSolidCube(0.4);
	glPopMatrix();
	glutSwapBuffers();

}

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("20011689 ¹Ú¹Î¼±");
	init();
	glutDisplayFunc(mydisplay);
	glutMouseFunc(mouseStatus);
	glutMainLoop();

}
