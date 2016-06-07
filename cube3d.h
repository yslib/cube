#pragma once
/*
*世界坐标到屏幕坐标的转换
*/

#include "all.h"
#include <vector>
#include "window.h"

class MainWindow;
class Cube
{
private:
	enum FlipIndex
	{
		Fface, Bface, Lface, Rface, Uface, Dface
	};
	static const double PI;
	/*
	*视图矩阵
	*				把世界坐标转化为平面上的投影
	*
	*				|v11 v12 v13 0|
	*				|v21 v22 v23 0|
	*				|0   v32 v33 0|
	*				|0   0   v43 1|
	*/
	float v11, v12, v13, v21, v22, v23, v32, v33, v43;
	//正方体的八个顶点
	float th, ph, rh;
	class Greater
	{
	private:
		Cube * cube;
	public:
		Greater(Cube * cube)
		{
			this->cube = cube;
		}
		bool operator()(int & a, int & b)
		{
			float z_a, z_b;
			z_a = cube->z_axis[cube->faces[a][0]] + cube->z_axis[cube->faces[a][1]] + cube->z_axis[cube->faces[a][2]] + cube->z_axis[cube->faces[a][3]];
			z_b = cube->z_axis[cube->faces[b][0]] + cube->z_axis[cube->faces[b][1]] + cube->z_axis[cube->faces[b][2]] + cube->z_axis[cube->faces[b][3]];
			return z_a < z_b;
		}
	};
	Greater *greater;

	Rotate * Flip10;	//旋转10度的矩阵，9次

	QSize screenSize;

	std::vector<Point3d> wordCoords;
	
	std::vector<int> U, D, L, R, F, B;
	
	std::vector<std::vector<int> * > FBLRUD;

	std::vector<int> drawOrder;
	
	float d;

	void inline swap(int a, int b, int c, int d, int direction);

	void Flip(FlipIndex face, int direction);

	void update();
public:

	Cube(QSize sceenSize = QSize(300, 400), double theta = PI / 4, double phy = PI / 4, double rho = 10.00);
	void toScreenCoord();
	void setTheta(float theta) { th = theta; update(); toScreenCoord(); }		//
	void setPhi(float phi) { ph = phi; update(); toScreenCoord(); }
	void setRho(float rho) { rh = rho; v43 = -rh; toScreenCoord(); }		 //透视程度

	void addTheta(float del) { setTheta(th + del); }
	void addPhi(float del) { setPhi(ph + del); }
	void addRho(float del) {setRho(rh + del);}

	void setDistance(float dist) { d = dist; toScreenCoord(); };		//视野远近
	void addDistance(float del) { setDistance(d + del); }
	void setColor(const std::vector<int> & color);
	void updateFlipSet(char face, int direction);
	const std::vector<int> & getOrder();
	//翻转
	void FlipU(int direction);
	void FlipD(int direction);
	void FlipL(int direction);
	void FlipR(int direction);
	void FlipF(int direction);
	void FlipB(int direction);
	void init();
	std::vector<QPointF> screenCoords;
	std::vector<std::vector<int> > faces;
	std::vector<int> FaceTable;
	std::vector<float> z_axis;
	//enum FaceColor{Green, Blue, Red, Orange, White, Yellow};

	//std::vector<QPointF> pointsScreenCoords;
};

class Cube3d :public QWidget
{
	Q_OBJECT
public:
	Cube3d(QWidget * parent = 0);

	~Cube3d();
public slots:

void FlipU() {
	for (int i = 0; i < 9; i++) { cube->FlipU(-1); update(); }cube->updateFlipSet('U', -1);
}
void FlipD() {
	 for (int i = 0; i < 9; i++) { cube->FlipD(1); update(); }cube->updateFlipSet('D', 1);
}
void FlipL() {
	 for (int i = 0; i < 9; i++) { cube->FlipL(-1); update(); }cube->updateFlipSet('L', -1);
}
void FlipR() {
	 for (int i = 0; i < 9; i++) { cube->FlipR(1); update(); }cube->updateFlipSet('R', 1);
}
void FlipF() {
	 for (int i = 0; i < 9; i++) { cube->FlipF(-1); update(); }cube->updateFlipSet('F', -1);
}
void FlipB() {
	 for (int i = 0; i < 9; i++) { cube->FlipB(1); update(); }cube->updateFlipSet('B', 1);
}

void Flipu() {
	 for (int i = 0; i < 9; i++) { cube->FlipU(1); update(); }cube->updateFlipSet('U', 1);
}
void Flipd() {
	 for (int i = 0; i < 9; i++) { cube->FlipD(-1); update(); }cube->updateFlipSet('D', -1);
}
void Flipl() {
	 for (int i = 0; i < 9; i++) { cube->FlipL(1); update(); }cube->updateFlipSet('L', 1);
}
void Flipr() {
	 for (int i = 0; i < 9; i++) { cube->FlipR(-1); update(); }cube->updateFlipSet('R', -1);
}
void Flipf() {
	 for (int i = 0; i < 9; i++) { cube->FlipF(1); update(); }cube->updateFlipSet('F', 1);
}
void Flipb() {
	 for (int i = 0; i < 9; i++) { cube->FlipB(-1); update(); }cube->updateFlipSet('B', -1);
}
	void flip();
	void FlipForDegbug()
	{
		flip();
		std::vector<int> faceColors(54);
		for (int i = 0; i < 54; i++)
			faceColors[i] = cube->faces[cube->FaceTable[i]][4];
		CubeInput cubeInput(faceColors);
		CubeSolver cubeSolver(cubeInput);
		std::vector<char> movement;
		std::vector<int> moveamount;
		cubeSolver.Solve(movement, moveamount);
		QString results;
		for (int i = 0; i < movement.size(); i++)
		{
			for (int j = 0; j < moveamount[i]; j++)
				results += movement[i];
		}
		operations->setText(SimplizeResult(results));
        //((MainWindow *)parentWidget())->sendData(results.toLatin1().data());
		flip();	
	}
	void CaptureColorStop();
	void CaptureColorStart();
private slots:
	
	void setCubeColor();
    
private:
	bool leftPressing;
	QPushButton * cube3dCaptureColorButton;
	QMutex q;
	Cube * cube;		//立方体
	int iX(float x) { return w / 2 + x; };
	int iY(float y) { return h / 2 - y; };
    QString SimplizeResult(const QString & result);
	int w, h;			//窗口的宽，得到中点，用来将逻辑坐标转换成屏幕坐标
	QPainter * painter;
	QPushButton * flipButton;
	QLineEdit * operations;
	QPoint prevMousePos;
	QTimer * timer;
protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
};
