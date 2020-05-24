#define FREEGLUT_STATIC
#define _USE_MATH_DEFINES
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <GL/glut.h>

using namespace std;

// 3次元ベクトルを扱うためのクラス
class Vector3d {
public:
	double x, y, z;
	Vector3d() { x = y = z = 0; }
	Vector3d(double _x, double _y, double _z) { x = _x; y = _y; z = _z; }
	void set(double _x, double _y, double _z) { x = _x; y = _y; z = _z; }

	// 長さを1に正規化する
	void normalize() {
		double len = length();
		x /= len; y /= len; z /= len;
	}

	// 長さを返す
	double length() { return sqrt(x * x + y * y + z * z); }

	// s倍する
	void scale(const double s) { x *= s; y *= s; z *= s; }

	// 加算の定義
	Vector3d operator+(Vector3d v) { return Vector3d(x + v.x, y + v.y, z + v.z); }

	// 減算の定義
	// ★加算の定義を参考に、減算のコードを記述してみる
	Vector3d operator-(Vector3d v) { return Vector3d(x - v.x, y - v.y, z - v.z); }

	// 内積の定義
	double operator*(Vector3d v) { return x * v.x + y * v.y + z * v.z; }

	// 外積の定義
	Vector3d operator%(Vector3d v) { return Vector3d(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }

	// 代入演算の定義
	Vector3d& operator=(const Vector3d& v) { x = v.x; y = v.y; z = v.z; return (*this); }

	// 加算代入の定義
	Vector3d& operator+=(const Vector3d& v) { x += v.x; y += v.y; z += v.z; return (*this); }

	// 減算代入の定義
	// ★加算代入の定義を参考に、減算代入のコードを記述してみる
	Vector3d& operator-=(const Vector3d& v) { x -= v.x; y -= v.y; z -= v.z; return (*this); }

	// 値を出力する
	void print() { printf("Vector3d(%f %f %f)\n", x, y, z); }
};

// マイナスの符号の付いたベクトルを扱えるようにするための定義 例：b=(-a); のように記述できる
Vector3d operator-(const Vector3d& v) { return(Vector3d(-v.x, -v.y, -v.z)); }

// ベクトルと実数の積を扱えるようにするための定義 例： c=5*a+2*b; c=b*3; のように記述できる
Vector3d operator*(const double& k, const Vector3d& v) { return(Vector3d(k * v.x, k * v.y, k * v.z)); }
Vector3d operator*(const Vector3d& v, const double& k) { return(Vector3d(v.x * k, v.y * k, v.z * k)); }

// ベクトルを実数で割る操作を扱えるようにするための定義 例： c=a/2.3; のように記述できる
Vector3d operator/(const Vector3d& v, const double& k) { return(Vector3d(v.x / k, v.y / k, v.z / k)); }


// 円柱を描画する関数（第3回のサンプルコードから引用）
void displayCylinder(float radius, float height, int nSlices) {
	// 天頂面
	const float deltaTheta = 2 * M_PI / (float)nSlices;
	glNormal3f(0, 1, 0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, height, 0);
	for (int i = 0; i <= nSlices; i++) {
		const float theta = deltaTheta * i;
		glVertex3f(radius * cosf(theta), height, radius * sinf(theta));
	}
	glEnd();
	// 底面
	glNormal3f(0, -1, 0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, 0, 0);
	for (int i = 0; i <= nSlices; i++) {
		const float theta = deltaTheta * i;
		glVertex3f(radius * cosf(theta), 0, radius * sinf(theta));
	}
	glEnd();
	// 側面
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

// 球体の情報を格納するクラス
class Sphere {
public:
	Vector3d position; // 中心位置
	float color[3];    // 描画色
	float degree;	   // 移動用
	float deltaX, deltaY, deltaZ; // 初期位置との差
	bool moveFlg = true; // 移動するかしないか
	float score = 10.f;       // 点数

	void setColor(float r, float g, float b) {
		color[0] = r; color[1] = g; color[2] = b;
	}

	// この球体を描画するメンバ関数
	void display() {
		glPushMatrix(); // 現在のモデル変換行列を退避しておく

		// 移動フラグが立っていれば球体を動かす
		if (moveFlg)
		{
			deltaX = 7 * cos(degree);
			deltaY = 5 * cos(degree);
			deltaZ = 3 * cos(degree);
			score = fmax(0, 10.f - (abs(deltaX) + abs(deltaY) + abs(deltaZ)));
		}
		glTranslated(deltaX, deltaY, deltaZ);

		// 座標の平行移動とスケール変換を施して球体を描画する
		glTranslated(position.x, position.y, position.z);
		glScaled(2, 2, 2);
		glutSolidSphere(1.3, 32, 32);

		glPopMatrix();  // 退避していたモデル変換行列を戻す
	}
};

// ゲームのタイトル画面とプレイ画面の切り替え用
bool g_GameStartFlg = false;

// ゲームのプレイ結果表示用
bool g_GameEndFlg = false;

// 視点変更用の角度
float g_HorizontalEyeDegree;

// 円柱の位置
float g_CylinderPosX = -20;

// 3つの球体を準備しておく
Sphere g_Sphere[3];

// 選択状態にある球体のID番号（0,1,2）を保持する。選択状態の球が無ければ-1とする。
int g_SelectedSphereID = -1;

// クリックされた3次元座標を保持する
Vector3d g_SelectedPos;

// ウィンドウサイズを保持する
int g_WindowWidth = 512;
int g_WindowHeight = 512;

// 選択した球体のID番号（0,1,2）を返す
// 選択した球体が無い場合は -1 を返す
int pickSphere(int x, int y) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// 照明効果を無くして単色で描画する
	glDisable(GL_LIGHTING);

	// 3つの球体を描画する
	for (int i = 0; i < 3; i++) {
		// RGBのR成分に球体のIDを設定する(unsigned byte型)
		glColor3ub(i, 0, 0);
		g_Sphere[i].display();
	}

	// ★授業スライドを参考に次のようなプログラムコードを追加する
	// ★glReadPixels 関数をつかって、クリックした位置が何色であるか取得する
	GLubyte c[3];
	glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, c);

	// ★取得した色を見て、どの球体を選択したか判定し、そのIDを return する。

	return (c[0] == 255) ? -1 : int(c[0]); // ★適切な値を返すようにする
}

// 描画関数
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (g_GameStartFlg)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);

		// 透視投影変換行列の設定
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(30.0, g_WindowWidth / (float)g_WindowHeight, 1.0, 1000.0);

		// カメラビュー座標への変換行列の設定
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(100 * cos(g_HorizontalEyeDegree), 10, 100 * sin(g_HorizontalEyeDegree), 0, 0, 0, 0, 1, 0);

		// 3つの球体を描画
		for (int i = 0; i < 3; i++) {
			// 球体ごとに色を変更する
			glMaterialfv(GL_FRONT, GL_DIFFUSE, g_Sphere[i].color);

			// 球体の描画を行う
			g_Sphere[i].display();
		}

		// クリア後に動かす円柱の描画
		glPushMatrix();
		float cy_c1[3] = { 0.5f, 0.7f, 0 };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, cy_c1);
		glTranslatef(g_CylinderPosX, 0, 0);
		glRotatef(90, 1, 0, 0);
		glRotatef(90, 0, 0, 1);
		displayCylinder(0.2f, 30.f, 32);
		glPopMatrix();

		// 球体の位置をガイドする円柱の描画
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

		// ゲームクリアしたら表示するプレイ結果
		if (g_GameEndFlg)
		{
			glDisable(GL_LIGHTING);
			glRasterPos3d(0, 10, 0);
			char res[256];
			float total = g_Sphere[0].score + g_Sphere[1].score + g_Sphere[2].score;
			sprintf_s(res, "Score: %0.2f / 30", total);

			// 文字列を1文字ずつ描画
			for (int i = 0; res[i] != '\0'; i++) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, res[i]);
			}

			// 点数に応じて文字を表示
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
		// タイトル画面の文字を表示

		// 透視投影変換行列の設定
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(30.0, g_WindowWidth / (float)g_WindowHeight, 1.0, 100.0);

		// カメラビュー座標への変換行列の設定
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

// 一定時間ごとに実行される関数
void timer(int val)
{
	g_Sphere[0].degree += 0.07f;
	g_Sphere[1].degree += 0.1f;
	g_Sphere[2].degree -= 0.06f;

	// ゲームが終わったら円柱を定位置に移動
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

// ウィンドウのサイズが変更されたときの処理
void resize(int w, int h) {
	if (h < 1) return;

	glViewport(0, 0, w, h);

	g_WindowWidth = w;
	g_WindowHeight = h;
}

// マウスカーソル位置に基づく選択処理
void MousePick(int x, int _y) {

	// マウスクリックで得られる座標は左下原点なので OpenGLの座標系と合わせるためにy座標を反転する
	const int y = g_WindowHeight - _y;

	g_SelectedSphereID = pickSphere(x, y);

	// 球が選択されていないなら何もしない
	if (g_SelectedSphereID == -1) return;

	// 選択した球体の動きを止める
	g_Sphere[g_SelectedSphereID].moveFlg = false;

	// 全ての球体の動きが止まったらゲームを終える
	if (g_Sphere[0].moveFlg == false && g_Sphere[1].moveFlg == false && g_Sphere[2].moveFlg == false)
	{
		g_GameEndFlg = true;
	}

	// クリックした場所の座標値（3次元座標）を取得する

	// ★授業スライドを参考に次のようなプログラムコードを追加する
	// ★現在のモデルビュー行列を取得する
	double M[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, M);

	// ★現在の透視投影行列の取得を取得する
	double P[16];
	glGetDoublev(GL_PROJECTION_MATRIX, P);

	// ★現在のビューポートの情報を取得
	int V[4];
	glGetIntegerv(GL_VIEWPORT, V);

	// ★マウスクリックした位置の奥行き情報（z値）を取得する
	float z;
	glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

	// ★上記の情報に基づいて、クリックした位置のワールド座標を取得する
	double objx, objy, objz;
	gluUnProject(x, y, z, M, P, V, &objx, &objy, &objz);

	// ★取得した値は、g_SelectedPos に格納しておく（←表示の時に使用する）
	g_SelectedPos.set(objx, objy, objz);
}

// マウスクリックのイベント処理
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

// マウスドラッグのイベント処理
void motion(int x, int y) {
	MousePick(x, y);
	glutPostRedisplay();
}

// キーが押されたときのイベント処理
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':
	case 'Q':
	case '\033':
		exit(0);  /* '\033' は ESC の ASCII コード */
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
	// 3つの球体の位置と色を設定しておく
	g_Sphere[0].position.set(-5, 0, 0);
	g_Sphere[1].position.set(0, 0, 0);
	g_Sphere[2].position.set(5, 0, 0);
	g_Sphere[0].setColor(1, 0.5, 0.5);
	g_Sphere[1].setColor(1, 1, 0.5);
	g_Sphere[2].setColor(0.3, 0.4, 0);

	glClearDepth(1000.0);
	glClearColor(1, 1, 1, 1); // 背景の色を白に設定

	// 照明の設定
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

	// 材質の設定
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
