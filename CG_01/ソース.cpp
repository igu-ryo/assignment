#define FREEGLUT_STATIC
#define _USE_MATH_DEFINES
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <GL/glut.h>

using namespace std;

// 3�����x�N�g�����������߂̃N���X
class Vector3d {
public:
	double x, y, z;
	Vector3d() { x = y = z = 0; }
	Vector3d(double _x, double _y, double _z) { x = _x; y = _y; z = _z; }
	void set(double _x, double _y, double _z) { x = _x; y = _y; z = _z; }

	// ������1�ɐ��K������
	void normalize() {
		double len = length();
		x /= len; y /= len; z /= len;
	}

	// ������Ԃ�
	double length() { return sqrt(x * x + y * y + z * z); }

	// s�{����
	void scale(const double s) { x *= s; y *= s; z *= s; }

	// ���Z�̒�`
	Vector3d operator+(Vector3d v) { return Vector3d(x + v.x, y + v.y, z + v.z); }

	// ���Z�̒�`
	// �����Z�̒�`���Q�l�ɁA���Z�̃R�[�h���L�q���Ă݂�
	Vector3d operator-(Vector3d v) { return Vector3d(x - v.x, y - v.y, z - v.z); }

	// ���ς̒�`
	double operator*(Vector3d v) { return x * v.x + y * v.y + z * v.z; }

	// �O�ς̒�`
	Vector3d operator%(Vector3d v) { return Vector3d(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }

	// ������Z�̒�`
	Vector3d& operator=(const Vector3d& v) { x = v.x; y = v.y; z = v.z; return (*this); }

	// ���Z����̒�`
	Vector3d& operator+=(const Vector3d& v) { x += v.x; y += v.y; z += v.z; return (*this); }

	// ���Z����̒�`
	// �����Z����̒�`���Q�l�ɁA���Z����̃R�[�h���L�q���Ă݂�
	Vector3d& operator-=(const Vector3d& v) { x -= v.x; y -= v.y; z -= v.z; return (*this); }

	// �l���o�͂���
	void print() { printf("Vector3d(%f %f %f)\n", x, y, z); }
};

// �}�C�i�X�̕����̕t�����x�N�g����������悤�ɂ��邽�߂̒�` ��Fb=(-a); �̂悤�ɋL�q�ł���
Vector3d operator-(const Vector3d& v) { return(Vector3d(-v.x, -v.y, -v.z)); }

// �x�N�g���Ǝ����̐ς�������悤�ɂ��邽�߂̒�` ��F c=5*a+2*b; c=b*3; �̂悤�ɋL�q�ł���
Vector3d operator*(const double& k, const Vector3d& v) { return(Vector3d(k * v.x, k * v.y, k * v.z)); }
Vector3d operator*(const Vector3d& v, const double& k) { return(Vector3d(v.x * k, v.y * k, v.z * k)); }

// �x�N�g���������Ŋ��鑀���������悤�ɂ��邽�߂̒�` ��F c=a/2.3; �̂悤�ɋL�q�ł���
Vector3d operator/(const Vector3d& v, const double& k) { return(Vector3d(v.x / k, v.y / k, v.z / k)); }


// �~����`�悷��֐��i��3��̃T���v���R�[�h������p�j
void displayCylinder(float radius, float height, int nSlices) {
	// �V����
	const float deltaTheta = 2 * M_PI / (float)nSlices;
	glNormal3f(0, 1, 0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, height, 0);
	for (int i = 0; i <= nSlices; i++) {
		const float theta = deltaTheta * i;
		glVertex3f(radius * cosf(theta), height, radius * sinf(theta));
	}
	glEnd();
	// ���
	glNormal3f(0, -1, 0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, 0, 0);
	for (int i = 0; i <= nSlices; i++) {
		const float theta = deltaTheta * i;
		glVertex3f(radius * cosf(theta), 0, radius * sinf(theta));
	}
	glEnd();
	// ����
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i <= nSlices; i++) {
		const float theta = deltaTheta * i;
		const float cosTheta = cosf(theta);
		const float sinTheta = sinf(theta);
		glNormal3f(cosTheta, 0, sinTheta);
		glVertex3f(radius * cosTheta, height, radius * sinTheta);
		glVertex3f(radius * cosTheta, 0, radius * sinTheta);
	}
	glEnd();
}

// ���̂̏����i�[����N���X
class Sphere {
public:
	Vector3d position; // ���S�ʒu
	float color[3];    // �`��F
	float degree;	   // �ړ��p
	float deltaX, deltaY, deltaZ; // �����ʒu�Ƃ̍�
	bool moveFlg = true; // �ړ����邩���Ȃ���
	float score = 10.f;       // �_��

	void setColor(float r, float g, float b) {
		color[0] = r; color[1] = g; color[2] = b;
	}

	// ���̋��̂�`�悷�郁���o�֐�
	void display() {
		glPushMatrix(); // ���݂̃��f���ϊ��s���ޔ����Ă���

		// �ړ��t���O�������Ă���΋��̂𓮂���
		if (moveFlg)
		{
			deltaX = 7 * cos(degree);
			deltaY = 5 * cos(degree);
			deltaZ = 3 * cos(degree);
			score = fmax(0, 10.f - (abs(deltaX) + abs(deltaY) + abs(deltaZ)));
		}
		glTranslated(deltaX, deltaY, deltaZ);

		// ���W�̕��s�ړ��ƃX�P�[���ϊ����{���ċ��̂�`�悷��
		glTranslated(position.x, position.y, position.z);
		glScaled(2, 2, 2);
		glutSolidSphere(1.3, 32, 32);

		glPopMatrix();  // �ޔ����Ă������f���ϊ��s���߂�
	}
};

// �Q�[���̃^�C�g����ʂƃv���C��ʂ̐؂�ւ��p
bool g_GameStartFlg = false;

// �Q�[���̃v���C���ʕ\���p
bool g_GameEndFlg = false;

// ���_�ύX�p�̊p�x
float g_HorizontalEyeDegree;

// �~���̈ʒu
float g_CylinderPosX = -20;

// 3�̋��̂��������Ă���
Sphere g_Sphere[3];

// �I����Ԃɂ��鋅�̂�ID�ԍ��i0,1,2�j��ێ�����B�I����Ԃ̋����������-1�Ƃ���B
int g_SelectedSphereID = -1;

// �N���b�N���ꂽ3�������W��ێ�����
Vector3d g_SelectedPos;

// �E�B���h�E�T�C�Y��ێ�����
int g_WindowWidth = 512;
int g_WindowHeight = 512;

// �I���������̂�ID�ԍ��i0,1,2�j��Ԃ�
// �I���������̂������ꍇ�� -1 ��Ԃ�
int pickSphere(int x, int y) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// �Ɩ����ʂ𖳂����ĒP�F�ŕ`�悷��
	glDisable(GL_LIGHTING);

	// 3�̋��̂�`�悷��
	for (int i = 0; i < 3; i++) {
		// RGB��R�����ɋ��̂�ID��ݒ肷��(unsigned byte�^)
		glColor3ub(i, 0, 0);
		g_Sphere[i].display();
	}

	// �����ƃX���C�h���Q�l�Ɏ��̂悤�ȃv���O�����R�[�h��ǉ�����
	// ��glReadPixels �֐��������āA�N���b�N�����ʒu�����F�ł��邩�擾����
	GLubyte c[3];
	glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, c);

	// ���擾�����F�����āA�ǂ̋��̂�I�����������肵�A����ID�� return ����B

	return (c[0] == 255) ? -1 : int(c[0]); // ���K�؂Ȓl��Ԃ��悤�ɂ���
}

// �`��֐�
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (g_GameStartFlg)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);

		// �������e�ϊ��s��̐ݒ�
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(30.0, g_WindowWidth / (float)g_WindowHeight, 1.0, 1000.0);

		// �J�����r���[���W�ւ̕ϊ��s��̐ݒ�
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(100 * cos(g_HorizontalEyeDegree), 10, 100 * sin(g_HorizontalEyeDegree), 0, 0, 0, 0, 1, 0);

		// 3�̋��̂�`��
		for (int i = 0; i < 3; i++) {
			// ���̂��ƂɐF��ύX����
			glMaterialfv(GL_FRONT, GL_DIFFUSE, g_Sphere[i].color);

			// ���̂̕`����s��
			g_Sphere[i].display();
		}

		// �N���A��ɓ������~���̕`��
		glPushMatrix();
		float cy_c1[3] = { 0.5f, 0.7f, 0 };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, cy_c1);
		glTranslatef(g_CylinderPosX, 0, 0);
		glRotatef(90, 1, 0, 0);
		glRotatef(90, 0, 0, 1);
		displayCylinder(0.2f, 30.f, 32);
		glPopMatrix();

		// ���̂̈ʒu���K�C�h����~���̕`��
		if (!g_GameEndFlg)
		{
			glPushMatrix();
			float cy_c2[3] = { 1.f, 0, 0 };
			glMaterialfv(GL_FRONT, GL_DIFFUSE, cy_c2);
			glTranslatef(9.f, 0, 0);
			glRotatef(90, 1, 0, 0);
			glRotatef(90, 0, 0, 1);
			displayCylinder(0.1f, 30.f, 32);
			glPopMatrix();
		}

		// �Q�[���N���A������\������v���C����
		if (g_GameEndFlg)
		{
			glDisable(GL_LIGHTING);
			glRasterPos3d(0, 10, 0);
			char res[256];
			float total = g_Sphere[0].score + g_Sphere[1].score + g_Sphere[2].score;
			sprintf_s(res, "Score: %0.2f / 30", total);

			// �������1�������`��
			for (int i = 0; res[i] != '\0'; i++) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, res[i]);
			}

			// �_���ɉ����ĕ�����\��
			if (total > 15)
			{
				glRasterPos3d(0, 7, 0);

				char res2[30];
				if (total > 25)
				{
					sprintf_s(res2, "Excellent!!!");
				}
				else if (total > 20)
				{
					sprintf_s(res2, "Great!!");
				}
				else
				{
					sprintf_s(res2, "Good!");
				}

				for (int i = 0; res2[i] != '\0'; i++) {
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, res2[i]);
				}
			}

		}
	}
	else
	{
		// �^�C�g����ʂ̕�����\��

		// �������e�ϊ��s��̐ݒ�
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(30.0, g_WindowWidth / (float)g_WindowHeight, 1.0, 100.0);

		// �J�����r���[���W�ւ̕ϊ��s��̐ݒ�
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0, 0, 30, 0, 0, 0, 0, 1, 0);

		glDisable(GL_LIGHTING);

		char title[30] = "Dango Game";
		glColor3f(0, 0.4, 0.8);
		glRasterPos3d(-1.3, 3, 10.f);
		for (int i = 0; title[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, title[i]);
		}

		glColor3f(1, 0, 0);
		char click[15] = "click to start";
		glRasterPos3d(-1, 0, 10.f);
		for (int i = 0; click[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, click[i]);
		}

		char desc[60] = "how to play: Click dango to stop. Rotate by 'f' or 'a' key.";
		glColor3f(0, 0, 0);
		glRasterPos3d(-5, -3, 10.f);
		for (int i = 0; desc[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, desc[i]);
		}
	}
	glutSwapBuffers();
}

// ��莞�Ԃ��ƂɎ��s�����֐�
void timer(int val)
{
	g_Sphere[0].degree += 0.07f;
	g_Sphere[1].degree += 0.1f;
	g_Sphere[2].degree -= 0.06f;

	// �Q�[�����I�������~�����ʒu�Ɉړ�
	if (g_GameEndFlg)
	{
		if (g_CylinderPosX < 9.5f)
		{
			g_CylinderPosX += 0.1f;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(10, timer, val);
}

// �E�B���h�E�̃T�C�Y���ύX���ꂽ�Ƃ��̏���
void resize(int w, int h) {
	if (h < 1) return;

	glViewport(0, 0, w, h);

	g_WindowWidth = w;
	g_WindowHeight = h;
}

// �}�E�X�J�[�\���ʒu�Ɋ�Â��I������
void MousePick(int x, int _y) {

	// �}�E�X�N���b�N�œ�������W�͍������_�Ȃ̂� OpenGL�̍��W�n�ƍ��킹�邽�߂�y���W�𔽓]����
	const int y = g_WindowHeight - _y;

	g_SelectedSphereID = pickSphere(x, y);

	// �����I������Ă��Ȃ��Ȃ牽�����Ȃ�
	if (g_SelectedSphereID == -1) return;

	// �I���������̂̓������~�߂�
	g_Sphere[g_SelectedSphereID].moveFlg = false;

	// �S�Ă̋��̂̓������~�܂�����Q�[�����I����
	if (g_Sphere[0].moveFlg == false && g_Sphere[1].moveFlg == false && g_Sphere[2].moveFlg == false)
	{
		g_GameEndFlg = true;
	}

	// �N���b�N�����ꏊ�̍��W�l�i3�������W�j���擾����

	// �����ƃX���C�h���Q�l�Ɏ��̂悤�ȃv���O�����R�[�h��ǉ�����
	// �����݂̃��f���r���[�s����擾����
	double M[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, M);

	// �����݂̓������e�s��̎擾���擾����
	double P[16];
	glGetDoublev(GL_PROJECTION_MATRIX, P);

	// �����݂̃r���[�|�[�g�̏����擾
	int V[4];
	glGetIntegerv(GL_VIEWPORT, V);

	// ���}�E�X�N���b�N�����ʒu�̉��s�����iz�l�j���擾����
	float z;
	glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

	// ����L�̏��Ɋ�Â��āA�N���b�N�����ʒu�̃��[���h���W���擾����
	double objx, objy, objz;
	gluUnProject(x, y, z, M, P, V, &objx, &objy, &objz);

	// ���擾�����l�́Ag_SelectedPos �Ɋi�[���Ă����i���\���̎��Ɏg�p����j
	g_SelectedPos.set(objx, objy, objz);
}

// �}�E�X�N���b�N�̃C�x���g����
void mouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN)
	{
		if (g_GameStartFlg)
		{
			MousePick(x, y);
		}
		else
		{
			g_GameStartFlg = true;
		}
	}
	glutPostRedisplay();
}

// �}�E�X�h���b�O�̃C�x���g����
void motion(int x, int y) {
	MousePick(x, y);
	glutPostRedisplay();
}

// �L�[�������ꂽ�Ƃ��̃C�x���g����
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':
	case 'Q':
	case '\033':
		exit(0);  /* '\033' �� ESC �� ASCII �R�[�h */
		break;
	case 'f':
		g_HorizontalEyeDegree += 0.2;
		break;
	case 'a':
		g_HorizontalEyeDegree -= 0.2;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}


void init() {
	// 3�̋��̂̈ʒu�ƐF��ݒ肵�Ă���
	g_Sphere[0].position.set(-5, 0, 0);
	g_Sphere[1].position.set(0, 0, 0);
	g_Sphere[2].position.set(5, 0, 0);
	g_Sphere[0].setColor(1, 0.5, 0.5);
	g_Sphere[1].setColor(1, 1, 0.5);
	g_Sphere[2].setColor(0.3, 0.4, 0);

	glClearDepth(1000.0);
	glClearColor(1, 1, 1, 1); // �w�i�̐F�𔒂ɐݒ�

	// �Ɩ��̐ݒ�
	float lightAmbientColor[] = { 0.2f, 0.2f, 0.2f, 0.0f };
	float lightDiffuseColor[] = { 3.f, 3.f, 3.f, 0.0f };
	float lightSpecularColor[] = { 0.4f, 0.4f, 0.4f, 0.0f };
	float lightPosition[] = { 0.0f, 30.0f, 30.0f, 0.0f };
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuseColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecularColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	// �ގ��̐ݒ�
	float specularColor[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float ambientColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float diffuseColor[] = { 1.f, 0.f, 0.f, 1.f };
	float shininess = 64.f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularColor);
	glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambientColor);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(g_WindowWidth, g_WindowHeight);
	glutCreateWindow("Mouse Picking");

	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutTimerFunc(10, timer, 0);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);

	init();

	glutMainLoop();

	return 0;
}
