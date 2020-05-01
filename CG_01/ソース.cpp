#define FREEGLUT_STATIC
#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <cmath>

void display(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	int k = 100;
	for (int j = 0; j < k; j++)
	{
		if (j % 2 == 0) glColor3d(0.0, 0.0, 1.0);
		else glColor3d(1.0, 1.0, 1.0);
		glBegin(GL_QUADS);
		for (int i = 0; i < 4; i++)
		{
			glVertex2d((double)(k - j) / k * cos((i * M_PI + j * 10) / 2.0), (double)(k - j) / k * sin((i * M_PI + j * 10) / 2.0));
		}
	}
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