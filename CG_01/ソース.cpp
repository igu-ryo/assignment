#define FREEGLUT_STATIC
#include <cstdlib>
#include <cmath>
#include <vector>
#include <GL/glut.h>

// 2次元ベクトルを扱うためのクラス
class Vector2d {
public:
	double x, y;
	Vector2d() { x = y = 0; }
	Vector2d(double _x, double _y) { x = _x; y = _y; }
	void set(double _x, double _y) { x = _x; y = _y; }

	// 長さを1に正規化する
	void normalize() {
		double len = length();
		x /= len; y /= len;
	}

	// 長さを返す
	double length() { return sqrt(x * x + y * y); }

	// s倍する
	void scale(const double s) { x *= s; y *= s; }

	// 加算の定義
	Vector2d operator+(Vector2d v) { return Vector2d(x + v.x, y + v.y); }

	// 減算の定義
	Vector2d operator-(Vector2d v) { return Vector2d(x - v.x, y - v.y); }

	// 内積の定義
	double operator*(Vector2d v) { return x * v.x + y * v.y; }

	// 代入演算の定義
	Vector2d& operator=(const Vector2d& v) { x = v.x; y = v.y; return (*this); }

	// 加算代入の定義
	Vector2d& operator+=(const Vector2d& v) { x += v.x; y += v.y; return (*this); }

	// 減算代入の定義
	Vector2d& operator-=(const Vector2d& v) { x -= v.x; y -= v.y; return (*this); }

	// 値を出力する
	void print() { printf("Vector2d(%f %f)\n", x, y); }
};

// マイナスの符号の付いたベクトルを扱えるようにするための定義 例：b=(-a); のように記述できる
Vector2d operator-(const Vector2d& v) { return(Vector2d(-v.x, -v.y)); }

// ベクトルと実数の積を扱えるようにするための定義 例： c=5*a+2*b; c=b*3; のように記述できる
Vector2d operator*(const double& k, const Vector2d& v) { return(Vector2d(k * v.x, k * v.y)); }
Vector2d operator*(const Vector2d& v, const double& k) { return(Vector2d(v.x * k, v.y * k)); }

// ベクトルを実数で割る操作を扱えるようにするための定義 例： c=a/2.3; のように記述できる
Vector2d operator/(const Vector2d& v, const double& k) { return(Vector2d(v.x / k, v.y / k)); }

// ================================================================================================


std::vector<Vector2d> g_ControlPoints; // 制御点を格納する

// ノットベクトルの要素数 （参考書にあわせて、要素数は10としている）
const int NUM_NOT = 15;
const int spline_d = 5; // スプラインの次数


// ノットベクトル
// この配列の値を変更することで基底関数が変化する。その結果として形が変わる。
// 下の例では、一定間隔で値が変化するので、「一様Bスプライン曲線」となる
double g_NotVector[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ,14 };

// クリックした位置と最も近い制御点のインデックス
int g_Near_i = 0;

// 制御点の移動フラグ
bool g_MoveFlg = false;

// グラフを描くかどうかのグラフ
bool g_GraphFlg = false;


// 基底関数 N{i,n}(t)の値を計算する
double getBaseN(int i, int n, double t) {
	if (n == 0) {
		// n が 0 の時だけ t の値に応じて 0 または 1 を返す
		if (t >= g_NotVector[i] && t < g_NotVector[i + 1]) {
			return 1.0;
		}
		return 0;
	}
	else {
		// ★ここに必要なプログラムコードを記述する
		// ★再帰（自分自身の関数 getBaseN を呼ぶ処理が必要）
		// ★係数を計算するときに、ノットが重なる（分母がゼロとなる）ときには、その項を無視する。
		double f, f1, f2;
		f1 = (g_NotVector[i + n] - g_NotVector[i]) != 0
			? (t - g_NotVector[i]) / (g_NotVector[i + n] - g_NotVector[i]) * getBaseN(i, n - 1, t)
			: 0;
		f2 = (g_NotVector[i + n + 1] - g_NotVector[i + 1]) != 0
			? (g_NotVector[i + n + 1] - t) / (g_NotVector[i + n + 1] - g_NotVector[i + 1]) * getBaseN(i + 1, n - 1, t)
			: 0;
		f = f1 + f2;
		return f;
	}
}

// 表示部分をこの関数で記入
void display(void) {
	glClearColor(1.0, 1.0, 1.0, 1.0);  // 消去色指定
	glClear(GL_COLOR_BUFFER_BIT);     // 画面消去

	// 制御点の描画
	glPointSize(5);
	glColor3d(0.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	for (unsigned int i = 0; i < g_ControlPoints.size(); i++) {
		glVertex2d(g_ControlPoints[i].x, g_ControlPoints[i].y);
	}
	glEnd();

	// 制御点を結ぶ線分の描画
	glColor3d(1.0, 0.0, 0.0);
	glLineWidth(1);
	glBegin(GL_LINE_STRIP);
	for (unsigned int i = 0; i < g_ControlPoints.size(); i++) {
		glVertex2d(g_ControlPoints[i].x, g_ControlPoints[i].y);
	}
	glEnd();

	// ★ ここにBスプライン曲線を描画するプログラムコードを入れる
	// ヒント1: 3次Bスプラインの場合は制御点を4つ入れるまでは何も描けない
	// ヒント2: パラメータtの値の取り得る範囲に注意

	const int spline_seg = (int)g_ControlPoints.size() - spline_d; // スプラインのセグメント数

	if (spline_seg > 0)
	{
		// Bスプライン曲線の描画
		glColor3d(0.0, 0.0, 0.0);
		glLineWidth(5);
		glBegin(GL_LINE_STRIP);

		for (double t = g_NotVector[spline_d]; t <= g_NotVector[spline_d + spline_seg]; t += 0.01)
		{
			Vector2d p;
			for (int i = 0; i <= spline_d + spline_seg - 1; i++)
			{
				p += getBaseN(i, spline_d, t) * g_ControlPoints[i];
			}
			glVertex2d(p.x, p.y);
		}

		glEnd();



		// 法線の描画
		const double width = 10.0;
		glColor3d(0.9, 0.9, 0.9);
		glLineWidth(1);
		
		const double delta_t = 0.0001;
		for (double t = g_NotVector[spline_d]; t <= g_NotVector[spline_d + spline_seg]; t += 0.001)
		{
			Vector2d p;
			for (int i = 0; i <= spline_d + spline_seg - 1; i++)
			{
				p += getBaseN(i, spline_d, t) * g_ControlPoints[i];
			}

			Vector2d p_;
			for (int i = 0; i <= spline_d + spline_seg - 1; i++)
			{
				p_ += getBaseN(i, spline_d, t + delta_t) * g_ControlPoints[i];
			}

			Vector2d dp_dt = (p_ - p) / delta_t;
			dp_dt.normalize();
			
			glBegin(GL_LINES);

			glVertex2d(p.x, p.y);
			glVertex2d(p.x + -dp_dt.y * width, p.y + dp_dt.x * width);

			glEnd();
		}



		// セグメント境界点の描画
		glColor3d(0.0, 0.0, 0.0);
		glBegin(GL_POINTS);

		for (int j = spline_d + 1; j <= spline_d + spline_seg - 1; j++)
		{
			double t = g_NotVector[j];
			Vector2d p;
			for (int i = 0; i <= spline_d + spline_seg - 1; i++)
			{
				p += getBaseN(i, spline_d, t) * g_ControlPoints[i];
			}
			glVertex2d(p.x, p.y);
		}

		glEnd();
	}



	// 基底関数のグラフの描画
	if (g_GraphFlg)
	{
		glClearColor(1.0, 1.0, 1.0, 1.0);  // 消去色指定
		glClear(GL_COLOR_BUFFER_BIT);     // 画面消去

		glColor3d(0.0, 0.0, 0.0);

		for (int i = 0; i < NUM_NOT - spline_d - 1; i++)
		{
			glBegin(GL_LINE_STRIP);

			for (double t = g_NotVector[i]; t <= g_NotVector[i + spline_d]; t += 0.1)
			{
				double n = getBaseN(i, spline_d, t);
				glVertex2d(20 + t * 30, 400 - n * 100);
			}

			glEnd();
		}
		
	}

	glutSwapBuffers();
}

void resizeWindow(int w, int h) {
	h = (h == 0) ? 1 : h;
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// ウィンドウ内の座標系設定
	// マウスクリックの座標と描画座標が一致するような正投影
	glOrtho(0, w, h, 0, -10, 10);

	glMatrixMode(GL_MODELVIEW);
}

// キーボードイベント処理
void keyboard(unsigned char key, int x, int y) {
	int i;
	double value;
	switch (key) {
	case 'q':
	case 'Q':
	case '\033':
		exit(0);  /* '\033' は ESC の ASCII コード */
		break;
	case 'c':
		scanf_s("%d:%lf", &i, &value);
		g_NotVector[i] = value;
		break;
	case 'g':
		g_GraphFlg ^= 1;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

// マウスイベント処理
void mouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		Vector2d clickPos(x, y);
		int min_d;

		switch (button) {
		case GLUT_LEFT_BUTTON:
			// クリックした位置に制御点を追加
			// ノット数を増やせばいくらでも制御点を追加できるが、今回はNUM_NOTの値で固定されているので
			// いくらでも追加できるわけではない
			if (g_ControlPoints.size() < NUM_NOT - spline_d - 1) {
				g_ControlPoints.push_back(Vector2d(x, y));
			}

			// クリックされた位置と一番近い制御点を選ぶ
			min_d = 1000000000;
			for (int i = 0; i < g_ControlPoints.size(); i++)
			{
				if (min_d > (g_ControlPoints[i] - clickPos).length())
				{
					min_d = (g_ControlPoints[i] - clickPos).length();
					g_Near_i = i;
				}
			}
			break;
		case GLUT_MIDDLE_BUTTON:
			break;
		case GLUT_RIGHT_BUTTON:
			// 末尾の制御点の削除
			if (!g_ControlPoints.empty()) {
				g_ControlPoints.pop_back();
			}
			break;
		default:
			break;
		}
		glutPostRedisplay(); // 再描画
	}
	else if (state == GLUT_UP)
	{
		if (g_MoveFlg)
		{
			g_ControlPoints[g_Near_i] = Vector2d(x, y);
			g_MoveFlg = false;
		}
	}
}

// マウスドラッグ処理
void motion(int x, int y)
{
	g_MoveFlg = true;
}

// メインプログラム
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);          // ライブラリの初期化
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE); // 描画モードの指定
	glutInitWindowSize(800, 800);  // ウィンドウサイズを指定
	glutCreateWindow(argv[0]);      // ウィンドウを作成
	glutDisplayFunc(display);       // 表示関数を指定
	glutReshapeFunc(resizeWindow);  // ウィンドウサイズが変更されたときの関数を指定
	glutKeyboardFunc(keyboard);     // キーボードイベント処理関数を指定
	glutMouseFunc(mouse);           // マウスイベント処理関数を指定
	glutMotionFunc(motion);         // マウスドラッグ処理関数を指定
	glutMainLoop();                 // イベント待ち
	return 0;
}