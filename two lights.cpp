#include <windows.h>
#include<gl/glut.h>
#include<gl/freeglut_ext.h>
#include<gl/freeglut_std.h>
#include<gl/freeglut.h>
#include<gl/GLU.h>
#include<gl/GL.h>
#include<stdio.h>
#include<stdlib.h>

void init(void)
{
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	GLfloat diffuse0[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat ambient0[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specular0[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light1_pos[] = { -1.0, -2.0, -3,0, 1.0 };
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular0);
}

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glutSolidSphere(0.5, 20, 16);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.7, 0.7, 0.5);
	glutSolidSphere(0.4, 20, 16);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.5, -0.6, 0.5);
	glutSolidSphere(0.2, 20, 16);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-45, 1, 0, 0);
	glTranslatef(-0.7, 0.5, 0.5);
	glutSolidCylinder(0.4, 0.5, 20, 16);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-45, 1, 0, 0);
	glTranslatef(-0.7, -1.5, 0.5);
	glutSolidCylinder(0.3, 0.4, 20, 16);
	glPopMatrix();

	glutSwapBuffers();
}
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(-1.5, 1.5, -1.5*(GLfloat)h / (GLfloat)w,
			1.5*(GLfloat)h / (GLfloat)w, -10.0, 10.0);
	else
		glOrtho(-1.5*(GLfloat)w / (GLfloat)h,
			1.5*(GLfloat)w / (GLfloat)h, -1.5, 1.5, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '0':
		glDisable(GL_LIGHT1);
		glutPostRedisplay();
		break;
	case '1':
		glDisable(GL_LIGHT0);
		glutPostRedisplay();
		break;
	case'2':
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glutPostRedisplay();
		break;
	case '3':
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glutPostRedisplay();
		break;
	case '4':
		exit(1);
		break;
	}

}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("20011689 ¹Ú¹Î¼±");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}
