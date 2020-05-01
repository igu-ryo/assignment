#define FREEGLUT_STATIC
#define _USE_MATH_DEFINES
#include <iostream>
#include <GL/glut.h>
#include <cmath>
using namespace std;

void kochCurve(double x1, double y1, double x2, double y2)
{
	double xx1, xx2, yy1, yy2, xx3, yy3, len, rad60;

	// 3“™•ª‚·‚é“_‚ÌÀ•W
	xx1 = x1 + (x2 - x1) * (1.0 / 3.0);
	yy1 = y1 + (y2 - y1) * (1.0 / 3.0);
	xx2 = x1 + (x2 - x1) * (2.0 / 3.0);
	yy2 = y1 + (y2 - y1) * (2.0 / 3.0);

	len = sqrt(pow(xx1 - xx2, 2.0) + pow(yy1 - yy2, 2.0));// 3“™•ª‚·‚é2“_ŠÔ‚Ì‹——£
	if (yy1 < yy2) rad60 = acos((xx2 - xx1) / len) + 60.0 * M_PI / 180.0;
	else rad60 = -acos((xx2 - xx1) / len) + 60.0 * M_PI / 180.0;
	if (len < 0.03) return;

	xx3 = xx1 + len * cos(rad60);
	yy3 = yy1 + len * sin(rad60);


	glVertex2d(x1, y1);
	kochCurve(x1, y1, xx1, yy1);
	glVertex2d(xx1, yy1);
	kochCurve(xx1, yy1, xx3, yy3);
	glVertex2d(xx3, yy3);
	kochCurve(xx3, yy3, xx2, yy2);
	glVertex2d(xx2, yy2);
	kochCurve(xx2, yy2, x2, y2);
	glVertex2d(x2, y2);
}

void display(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3d(1.0, 0.0, 1.0);
	glBegin(GL_LINE_STRIP);

	kochCurve(-0.5, -0.5, 0.5, -0.5);

	glEnd();
	glFlush();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitWindowSize(400, 400);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}