#include "cube2d.h"
#include "all.h"
#include "QPainter"
#include "window.h" 
#include "QGridLayout"
#include "QPen"
#include "CubeSolver.h"


#define ABS(x) ((x)>0?(x):(-(x)))

Cube2d::Cube2d(const QVector<Video *> videos,QWidget * parent /* = 0 */) :QWidget(parent)
{
	this->videos = videos;
	timer = new QTimer(this);

	connect(timer, SIGNAL(timeout()), this, SLOT(refreshCude()));
	
	//cubeLabel->setGeometry(0, 0, 500, 500);
	faceColors.resize(54);
	
	margin = (FlatLenth - 3*SquareLenth) / 3;

	FBLRUD = new std::vector<QPoint>;

	FBLRUD->push_back(QPoint(2*FlatMargin + FlatLenth,2*FlatMargin+FlatLenth));					//f
	FBLRUD->push_back(QPoint(4 * FlatMargin + 3*FlatLenth, 2 * FlatMargin + FlatLenth));			//b
	FBLRUD->push_back(QPoint(1 * FlatMargin + 0*FlatLenth, 2 * FlatMargin + FlatLenth));			//l	
	FBLRUD->push_back(QPoint(3 * FlatMargin + 2*FlatLenth, 2 * FlatMargin + FlatLenth));			//r
	FBLRUD->push_back(QPoint(2 * FlatMargin + FlatLenth, 1 * FlatMargin + 0 * FlatLenth));			//u
	FBLRUD->push_back(QPoint(2 * FlatMargin + FlatLenth, 3 * FlatMargin + 2*FlatLenth));			//d

	//初始化square 的矩形
	for (int k = 0; k < 6; k++)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				int leftTopX = FBLRUD->operator[](k).x();
				int leftTopY = FBLRUD->operator[](k).y();
				squareRects.push_back(QRectF(
					leftTopX + (j + 1)*margin + j*SquareLenth,
					leftTopY + (i + 1)*margin + i*SquareLenth,
					SquareLenth,
					SquareLenth));
			}
		}
	}

	cubeImage = new QImage(QSize(400,400),QImage::Format_ARGB32);
	cubeLabel = new QLabel(this);
	cubeLabel->setGeometry(0, 0, 400, 400);

	painter = new QPainter(this);

	//file = new QFile("C:\\Users\\ysl\\color.txt");
	//if (file->open(QIODevice::ReadWrite | QIODevice::Text) == false)
	//{
	//	QMessageBox::warning(this, tr("can not open file"), tr("can not open file"));
	//}

	//cubeLabel->setGeometry(100, 100, 1500, 300);
	videoComboBox = new QComboBox(this);
	videoComboBox->addItem("Camera 1");
	videoComboBox->addItem("Camera 2");
	videoComboBox->addItem("Camera 3");
	videoComboBox->addItem("Camera 4");

	videosDetectAreaArray.resize(4);
	for (int i = 0; i < 6;i++)
		areaTable.push_back(QRect((*FBLRUD)[i],(*FBLRUD)[i]+QPoint(FlatLenth,FlatLenth)));

	/*布局*/
	QHBoxLayout * hLayout = new QHBoxLayout(this);
	hLayout->addWidget(cubeLabel);
	hLayout->addStretch();
	QVBoxLayout * vLayout = new QVBoxLayout(this);
	videoLabel = new QLabel(tr("Camera:"),this);
	vLayout->addWidget(videoLabel);
	vLayout->addWidget(videoComboBox);
	vLayout->addStretch();

	hLayout->addLayout(vLayout);
	setLayout(hLayout);
	resize(QSize(400, 400));

	//cube2dThread = new Cube2dThread(cubeImage,
	//	cubeLabel, (((MainWindow *)parentWidget())->getCubePosition()).rgbs,
	//	faceColors,
	//	FBLRUD, margin, this, painter);
	//cube2dThread->start();
    timer->start(200);
}
std::set<int> & Cube2d::getSelectedDetectPoints()
{
    return selected;
}
DetectAreaIndex * Cube2d::getDetectAreaIndex(int videoNumber)
{
    if((size_t)videoNumber > videosDetectAreaArray.size() || videoNumber < 0)
        return NULL;
    return &videosDetectAreaArray[videoNumber];
}
void Cube2d::refreshCude()
{
	update();
}
const std::vector<int>& Cube2d::getCubeColor()
{
	// TODO: 在此处插入 return 语句
	return faceColors;
}
void Cube2d::mousePressEvent(QMouseEvent *event)
{
	//根据选择哪个摄像头
	//判断鼠标点击在哪个方格里，如果在合法的方格里，把这个方格的序号添加到video 类中的detectIndex中
	//如果已经存在在detectIndex中，那么去掉这个
	//并且重新绘制这个方格的外形

	QPoint point = event->pos();
	int videoIndex = videoComboBox->currentIndex();
	//QMessageBox::warning(this, tr("debug"), tr("%1").arg(videoIndex));

	for (int i = 0; i < 6; i++)
	{
		if (areaTable[i].contains(point) == true)		//判断在哪个区域中
		{
			//QMessageBox::warning(this, tr("debug"), tr("%1").arg(i));
			//计算在区域的哪个格子里
			int x = (point.x() - areaTable[i].left()) / (FlatLenth/3);
			int y = (point.y() - areaTable[i].top()) / (FlatLenth/3);
			int index =i*9+ 3 * y +x;
			//QMessageBox::warning(this, tr("debug"), tr("%1 %2 %3").arg(x).arg(y).arg(index));
			if (selected.find(index) == selected.end())			//还没有被选择，选择
			{
				//QMessageBox::warning(this, tr("debug"), tr("%1").arg(index));
				selected.insert(index);
				videosDetectAreaArray[videoIndex].push_back(index);

				videos[videoIndex]->setDetectPoints(&videosDetectAreaArray[videoIndex]);
				//更新样式
				updateEdgeStyle(index, videoIndex,1);
				break;
			}
			else           //已经被选择了，取消选择
			{
				selected.erase(index);
				for (DetectAreaIndex::iterator itr = videosDetectAreaArray[videoIndex].begin(); itr != videosDetectAreaArray[videoIndex].end(); ++itr)
				{
					if (*itr == index)
					{
						videosDetectAreaArray[videoIndex].erase(itr);
						break;
					}
				}
				videos[videoIndex]->setDetectPoints(&videosDetectAreaArray[videoIndex]);
				//更新样式
				updateEdgeStyle(index, videoIndex,0);
				break;
			}
		}
	}
}
void Cube2d::updateEdgeStyle(int areaIndex, int videoIndex,int style)
{
	/*1 显示, 0 清空*/
	static const QColor colorForEdge[4] = {
		Qt::red,Qt::green,Qt::blue,Qt::cyan
	};
	painter->begin(this);
	painter->setPen(QPen(Qt::blue, 4, Qt::DashLine));
	painter->setBrush(Qt::red);
	if (style == 1)
	{
		painter->setBrush(colorForEdge[videoIndex]);		//根据选择的摄像头的不同取不同的颜色
	}
	else
	{
		painter->setBrush(palette().background().color());		//背景色
	}
	//QMessageBox::warning(this, tr("debug"),tr("afds"));
	painter->drawRect((squareRects[areaIndex]).adjusted(10,10,10,10));		//增大一圈
	painter->end();
	update();
}
void Cube2d::tmpUpdate()
{
	
}
void Cube2d::paintEvent(QPaintEvent * event)
{
	const std::vector<sRGB> & rgbs = (((MainWindow *)parentWidget())->getCubePosition()).rgbs;
	//if (video != NULL)
	//	rgbs = video->getRGB();
	//for (int i = rgbs.size(); i < 9; i++)
	//	rgbs.push_back(sRGB(255, 255, 255));

	//textEdit->clear();
	//QString colorVal;
	int colorTag;
	QColor color;
	int r, g, b;
	int h;
	double s, v;
	char colorrgb[4];
	colorrgb[3] = '\0';
	((MainWindow *)parentWidget())->setTextEditContent(tr(""));
	//QTextStream in(file);
	for (int k = 0; k < 6; k++)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				r=rgbs[k * 9 + i * 3 + j].r;
				g=rgbs[k * 9 + i * 3 + j].g;
				b=rgbs[k * 9 + i * 3 + j].b;
				bgr2hsv(rgbs[k*9+i * 3 + j].b, rgbs[k * 9+i * 3 + j].g, rgbs[k * 9 + i * 3 + j].r, h, s, v);
				color = threshold(h, s, v, colorTag);
				//color = thresholdRGB(r,g,b,colorTag);
				faceColors[k * 9 + i * 3 + j] = colorTag;
				int leftTopX= FBLRUD->operator[](k).x();
				int leftTopY = FBLRUD->operator[](k).y();
				colorrgb[0] = r; colorrgb[1] = g; colorrgb[2] = b;
				QString colorVal(colorrgb);
				((MainWindow *)parentWidget())->sendData(colorVal);
				//in << tr("%1 %2 %3 %4 %5 %6\n").arg(rgbs[k * 9 + i * 3 + j].r).arg(rgbs[k * 9 + i * 3 + j].g).arg(rgbs[k * 9 + i * 3 + j].b).arg(h).arg(s).arg(v);
				//in << tr("%1%2%3").arg((char)r).arg((char)g).arg((char)b);
				//colorVal += QString(tr("%1 %2 %3\r\n").arg(h).arg(s).arg(v));
				if(k==0)
				((MainWindow *)parentWidget())->appendTextEditContent(tr("(%1,%2,%3)").arg(h).arg(s).arg(v));
				painter->begin(this);
				painter->setPen(color);
				painter->setBrush(color);
				painter->drawRect(QRectF(
					leftTopX+(j + 1)*margin + j*SquareLenth, 
					leftTopY+(i + 1)*margin + i*SquareLenth, 
					SquareLenth, 
					SquareLenth));
				painter->end();	
			}
		}
	}
}
QColor threshold(int h, double s, double v, int &ColorTag)
{
	if ((ABS(h - Red_H) < Error || ABS(h)<Error )/*&& s > 0.5*/)
	{
		ColorTag = FaceColor::Red;
		return QColor(255, 0, 0);
	}	/*Red*/
	else if (ABS(h - Green_H) < Error /*&& s > 0.5*/)
	{
		ColorTag = FaceColor::Green;
		return QColor(34, 135, 81);
	}/*Green*/
	else if (ABS(h - Yellow_H) < Error/* && s > 0.4*/)
	{
		ColorTag = FaceColor::Yellow;
		return QColor(241, 216, 111);
	}/*Yellow*/
	else if (ABS(h - Blue_H) < Error /*&& s > 0.45*/)
	{
		ColorTag = FaceColor::Blue;
		return QColor(0, 0, 255);
	}/*Blue*/
	else if (ABS(h - Orange_H) < Error /*&& s > 0.45*/)
	{
		ColorTag = FaceColor::Orange;
		return QColor(252, 123, 73);
	}/*Orange*/
	else if (s < 0.05 && v>0.83)
	{
		ColorTag = FaceColor::White;
		return QColor(255, 255, 255);
	}/*White*/
	ColorTag = -1;
	return QColor(0,0,0);/*Black*/
}
QColor thresholdRGB(int r, int b, int g, int & ColorTag)
{
	if ((r > Green_Rmin && r < Green_Rmax) && (b > Green_Bmin && b < Green_Bmax) && (g > Green_Gmin && g < Green_Gmax))
	{
		ColorTag = FaceColor::Green;
		return QColor(34, 135, 81);
	}
	else if ((r > Blue_Rmin && r < Blue_Rmax) && (b > Blue_Bmin && b < Blue_Bmax) && (g > Blue_Gmin && g < Blue_Gmax))
	{
		ColorTag = FaceColor::Blue;
		return QColor(0, 0, 255);
	}
	else if ((r > Red_Rmin && r < Red_Rmax) && (b > Red_Bmin && b < Red_Bmax) && (g >Red_Gmin && g < Red_Gmax))
	{
		ColorTag = FaceColor::Red;
		return QColor(255, 0, 0);
	}
	else if ((r > Orange_Rmin && r < Orange_Rmax) && (b > Orange_Bmin && b < Orange_Bmax) && (g > Orange_Gmin && g < Orange_Gmax))
	{
		ColorTag = FaceColor::Orange;
		return QColor(252, 123, 73);
	}
	else if ((r > White_Rmin && r < White_Rmax) && (b >White_Bmin && b < White_Bmax) && (g > White_Gmin && g < White_Gmax))
	{
		ColorTag = FaceColor::White;
		return QColor(255, 255, 255);
	}
	else if ((r > Yellow_Rmin && r < Yellow_Rmax) && (b > Yellow_Bmin && b < Yellow_Bmax) && (g > Yellow_Gmin && g < Yellow_Gmax))
	{
		ColorTag = FaceColor::Yellow;
		return QColor(241, 216, 111);
	}
	ColorTag = -1;
	return QColor(0, 0, 0);/*Black*/
}
Cube2d::~Cube2d()
{
	delete FBLRUD;
	delete cubeImage;
}
Cube2dThread::Cube2dThread(QImage * img,QLabel *lbl,
	const std::vector<sRGB> & rgb,
	std::vector<int> & fc,
	std::vector<QPoint> * fblrud,float mg,Cube2d * c2,QPainter * pter):rgbs(rgb),
	faceColors(fc),
	cubeImage(img),FBLRUD(fblrud),cubeLabel(lbl),margin(mg),cube2d(c2)
{
	painter = pter;
	stopped = false;
}
void Cube2dThread::run()
{
	while (stopped == false)
	{
		QString colorVal;
		int colorTag;

		for (int k = 0; k < 6; k++)
		{
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					int h;
					double s, v;
					bgr2hsv(rgbs[k * 9 + i * 3 + j].b, rgbs[k * 9 + i * 3 + j].g, rgbs[k * 9 + i * 3 + j].r, h, s, v);

					QColor color = threshold(h, s, v, colorTag);
					faceColors[k * 9 + i * 3 + j] = colorTag;
					int leftTopX = FBLRUD->operator[](k).x();
					int leftTopY = FBLRUD->operator[](k).y();

					//colorVal += QString(tr("%1 %2 %3\r\n").arg(h).arg(s).arg(v));

					if (cubeImage == NULL)
						qDebug() << "asdf";
					painter->begin(cubeImage);
					painter->setPen(color);
					painter->setBrush(color);
					painter->drawRect(QRectF(
						leftTopX + (j + 1)*margin + j*SquareLenth,
						leftTopY + (i + 1)*margin + i*SquareLenth,
						SquareLenth,
						SquareLenth));
					painter->end();
				}
			}
		}
		//((MainWindow *)parentWidget())->setTextEditContent(colorVal);
		cubeLabel->setPixmap(QPixmap::fromImage(*cubeImage));
	}
	stopped = false;
}

//rgb2hsv
//max = max(R, G, B)
//2: min = min(R, G, B)
//3 : if R = max, H = (G - B) / (max - min)
//4 : if G = max, H = 2 + (B - R) / (max - min)
//5 : if B = max, H = 4 + (R - G) / (max - min)
//6 :
//	7 : H = H * 60
//	8 : if H < 0, H = H + 360
//	9 :
//	10 : V = max(R, G, B)
//	11 : S = (max - min) / max
void bgr2hsv(uchar b, uchar g, uchar r, int & h, double &s, double &v)
{
	double _min = 9999;
	double _max = -1;
	if (b > g)
	{
		if (g > r)
		{
			_min = r;
			_max = b;
		}
		else
		{
			_min = g;
			if (b > r)
				_max = b;
			else
				_max = r;
		}
	}
	else
	{
		if (b > r)
		{
			_min = r;
			_max = g;
		}
		else
		{
			_min = b;
			if (r > g)
				_max = r;
			else
				_max = g;
		}
	}
	double _h;
	if (r == _max)_h = (double)(g - b) / (_max - _min);
	if (g == _max)_h = 2 + (double)(b - r) / (_max - _min);
	if (b == _max)_h = 4 + (double)(r - g) / (_max - _min);
	_h *= 60;
	while (_h < 0)_h += 360;
	v = _max / 255;
	s = (_max - _min) / _max;
	h = _h;
}
