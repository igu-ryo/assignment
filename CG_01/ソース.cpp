#define FREEGLUT_STATIC
#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <math.h>

void display(void);
void donut(int);


int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitWindowSize(400, 400);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}

void display(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3d(0.0, 1.0, 0.0);

	glBegin(GL_LINES);
	glVertex2d(2, 3);
	glVertex2d(200, 30);
	//donut(10);
	glEnd();

	glFlush();
}

void donut(int width)
{
	for (int i = 0; i < 360; i++)
	{
		double x1 = cos(i * M_PI / 180.0);
		double y1 = sin(i * M_PI / 180.0);
		glVertex2d(x1 * 0.5, y1 * 0.5);
		double x2 = cos(i * M_PI / 180.0);
		double y2 = sin(i * M_PI / 180.0);
		glVertex2d(x2 * 0.5, y2 * 0.5);
	}
}