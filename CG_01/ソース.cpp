#define FREEGLUT_STATIC
#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <cmath>

class kochCurve
{
public:
	static int limit; // ���O�p�`��`���J��Ԃ��񐔁i1�ȏ�j
	int nowTime;

	kochCurve(int lastTime)
	{
		nowTime = lastTime + 1;
	}

	void draw(double x1, double y1, double x2, double y2)
	{
		if (nowTime > limit) return;

		// 3��������2�_AB�̍��W
		double xx1 = x1 + (x2 - x1) * (1.0 / 3.0);
		double yy1 = y1 + (y2 - y1) * (1.0 / 3.0);
		double xx2 = x1 + (x2 - x1) * (2.0 / 3.0);
		double yy2 = y1 + (y2 - y1) * (2.0 / 3.0);

		// 3��������2�_AB�Ԃ̋���
		double len = sqrt(pow(xx1 - xx2, 2.0) + pow(yy1 - yy2, 2.0));

		// �V���ɂƂ�_��C�Ƃ����Ƃ��A�pCAB��60�x�ɂȂ�悤�ȓ_C�̕Ίp�����Ƃ߂�
		double rad;
		if (yy1 < yy2) rad = acos((xx2 - xx1) / len) + 60.0 * M_PI / 180.0;
		else rad = - acos((xx2 - xx1) / len) + 60.0 * M_PI / 180.0;

		// �_C�̍��W
		double xx3 = xx1 + len * cos(rad);
		double yy3 = yy1 + len * sin(rad);

		// ���߂�3�_���܂�5�_�ɂ����āA�ċA�I�Ɋ֐����Ăяo���A�_���J��Ԃ���葱����
		glVertex2d(x1, y1);
		kochCurve obj1(nowTime);
		obj1.draw(x1, y1, xx1, yy1);
		glVertex2d(xx1, yy1);
		kochCurve obj2(nowTime);
		obj2.draw(xx1, yy1, xx3, yy3);
		glVertex2d(xx3, yy3);
		kochCurve obj3(nowTime);
		obj3.draw(xx3, yy3, xx2, yy2);
		glVertex2d(xx2, yy2);
		kochCurve obj4(nowTime);
		obj4.draw(xx2, yy2, x2, y2);
		glVertex2d(x2, y2);
	}
};

int kochCurve::limit = 4;

void display(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3d(1.0, 0.0, 1.0);
	glBegin(GL_LINE_STRIP);
	kochCurve obj1(0);
	obj1.draw(0.7, -0.3, -0.7, -0.3);
	kochCurve obj2(0);
	obj2.draw(-0.7, -0.3, 0.0, 1.4 * sin(M_PI / 3.0) - 0.3);
	kochCurve obj3(0);
	obj3.draw(0.0, 1.4 * sin(M_PI / 3.0) - 0.3, 0.7, -0.3);
	glEnd();
	glFlush();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitWindowSize(750, 750);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}