#pragma once

#include "all.h"
#include "video.h"
#include <set>
#include "CubeSolver.h"
void bgr2hsv(uchar  b, uchar a, uchar r, int & h, double &s, double &v);
QColor thresholdRGB(int r, int b, int g, int & ColorTag);
QColor  threshold(int n, double s, double v, int &ColorTag);
enum 
{
	SquareLenth = 20, 

	FlatLenth = 70, 

	FlatMargin = 5
};
//enum {
//	//Red = 0, Green = 1, Yellow = 2, Blue = 3, White = 4, Orange = 5
//	Green, Blue, Red, Orange, White, Yellow
//};
enum {

	Red_H = 355,

	Green_H = 145,

	Yellow_H = 50,

	Blue_H = 220, 

	Orange_H = 10, 

	Error = 15
};

class Cube2dThread;

class Cube2d;

class Cube2d :public QWidget
{
	Q_OBJECT
public:
	Cube2d(const QVector<Video *> videos,QWidget * parent = 0);
    
    std::set<int> & getSelectedDetectPoints();
    
    DetectAreaIndex * getDetectAreaIndex(int videoNumber);

	~Cube2d();

protected:

	void paintEvent(QPaintEvent * event);

	void tmpUpdate();

private:

	std::vector<QPoint> * FBLRUD;			//fblrdu面的左上角的坐标
	//除白色之外，其他颜色的S值都在0.5之上，白色的S值在0.1之下，且V值接近于1

	QTimer *timer;

	QFile * file;

	Cube2dThread * cube2dThread;

	QPainter * painter;

	QLabel * cubeLabel;

	QImage * cubeImage;

	QLabel * videoLabel;

	QComboBox * videoComboBox;

	QLabel * serialLabel;

	QComboBox * serialPortCombBox;

	std::vector<DetectAreaIndex> videosDetectAreaArray;

	QVector<QRectF> squareRects;		//记录54 个小正方形的位置

	QVector<QRectF> areaTable;	//区域检索表

	std::set<int> selected;

	QVector<Video *> videos;

	std::vector<int> faceColors;
	
	float margin;

	void updateEdgeStyle(int areaIndex,int videoIndex,int style);

public slots:

	void refreshCude();

	const std::vector<int> & getCubeColor();

protected:

	void mousePressEvent(QMouseEvent *event);
};


class Cube2dThread : public QThread
{
public:
	//Cube2dThread() {}
	Cube2dThread(QImage * img, QLabel *lbl,
		const std::vector<sRGB> & rgb,
		std::vector<int> & fc,
		std::vector<QPoint> * fblrud, float mg,Cube2d * c2,QPainter * pter);

	~Cube2dThread() {}

	void stop()
	{
		stopped = true;
	}
protected:

	void run();

private:

	volatile bool stopped;

	float margin;

	QImage * cubeImage;

	const std::vector<sRGB> & rgbs;

	std::vector<int> & faceColors;

	std::vector<QPoint> * FBLRUD;

	QLabel * cubeLabel;

	QPainter * painter;

	Cube2d * cube2d;
};