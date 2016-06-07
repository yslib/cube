#include "cube3d.h"
#include "window.h"
#include "QRegExp"
#include <map>
#include <QApplication>

const double Cube::PI = 3.1415926535;
Cube::Cube(QSize sceenSize, double theta /* = PI/4 */, double phi /* = PI/4 */, double rho /* = 10.00 */)
{
	th = theta;
	ph = phi;
	rh = rho;
	update();
	this->screenSize = sceenSize;
	FaceTable.resize(54);		//54个面编号为0-53
	FBLRUD.resize(6);			//6个旋转集
	screenCoords.resize(216);	//屏幕坐标
	z_axis.resize(216);			//屏幕坐标，Z轴
	faces.resize(54);			//每个小块的顶点坐标及颜色		前四个为顶点坐标索引，最后一个是颜色
	drawOrder.resize(54);
	greater = new Greater(this);
	init();
	Flip10 = new Rotate(1.570796 / 9);
	
	//wordCoords.resize(56);
	d = rh / 1.5 * (sceenSize.width() < sceenSize.height() ? sceenSize.width() : sceenSize.height()) / std::sqrt(12);
}
void Cube::setColor(const std::vector<int> & color)
{
	for (int i = 0; i < 54; i++)
		faces[i][4] = color[i];
}
void Cube::toScreenCoord()
{
	for (int i = 0; i < wordCoords.size(); i++)
	{
		Point3d & w = wordCoords[i];
		float x = v11*w.x + v21*w.y;
		float y = v12*w.x + v22*w.y + v32*w.z;
		float z = v13*w.x + v23*w.y + v33*w.z + v43;
		screenCoords[i] = QPointF(-d*x / z, -d*y / z);
		z_axis[i] = z;
	}
}
//
//a<-b<-c<-d<-a if direction > 0
//d<-c<-b<-a<-d
void Cube::swap(int a, int b, int c, int d, int direction)
{
	if (direction < 0)
	{
		int tmp = FaceTable[a];
		FaceTable[a] = FaceTable[b];
		FaceTable[b] = FaceTable[c];
		FaceTable[c] = FaceTable[d];
		FaceTable[d] = tmp;
	}
	else
	{
		int tmp = FaceTable[d];
		FaceTable[d] = FaceTable[c];
		FaceTable[c] = FaceTable[b];
		FaceTable[b] = FaceTable[a];
		FaceTable[a] = tmp;
	}

}
void Cube::Flip(FlipIndex face, int direction)
{
	QElapsedTimer t;
	t.start();
	while (t.elapsed() < 60)
		QCoreApplication::processEvents();
	for (int i = 0; i < (*FBLRUD[face]).size(); i++)
	{
		int faceIndex = FaceTable[(*FBLRUD[face])[i]];
		for (int j = 0; j < 4; j++)
		{
			int pointIndex = faces[faceIndex][j];
			if (face == Uface || face == Dface)
				wordCoords[pointIndex] = (*Flip10).RotateZ(wordCoords[pointIndex], direction);
			else if (face == Lface || face == Rface)
				wordCoords[pointIndex] = (*Flip10).RotateY(wordCoords[pointIndex], direction);
			else
				wordCoords[pointIndex] = (*Flip10).RotateX(wordCoords[pointIndex], direction);
		}
	}
}
void Cube::FlipU(int direction)
{
	Flip(Uface, direction);
	toScreenCoord();
}
void Cube::FlipD(int direction)
{
	Flip(Dface, direction);

	toScreenCoord();

}
void Cube::FlipL(int direction)
{
	Flip(Lface, direction);

	toScreenCoord();
}
void Cube::FlipR(int direction)
{
	Flip(Rface, direction);
	toScreenCoord();
}
void Cube::FlipF(int direction)
{
	Flip(Fface, direction);
	toScreenCoord();
}
void Cube::FlipB(int direction)
{
	Flip(Bface, direction);

	toScreenCoord();

}
void Cube::init()
{
	for (int i = 0; i < 54; i++)
	{
		FaceTable[i] = i;
	}
	FBLRUD[0] = &F; FBLRUD[1] = &B; FBLRUD[2] = &L; FBLRUD[3] = &R; FBLRUD[4] = &U; FBLRUD[5] = &D;
	float lenth = 1;
	float x, y, z;
	//F面 面标号 0-8 点坐标0-35 4个一组，逆时针排列
	x = lenth;
	for (int i = 0; i < 3; i++)
	{
		z = lenth - 0.6667*lenth*i;
		for (int j = 0; j < 3; j++)
		{
			y = -lenth + j*0.6667*lenth;
			wordCoords.push_back(Point3d(x, y, z));
			wordCoords.push_back(Point3d(x, y, z - 0.6667*lenth));
			wordCoords.push_back(Point3d(x, y + 0.6667*lenth, z - 0.6667*lenth));
			wordCoords.push_back(Point3d(x, y + 0.6667*lenth, z));
		}
	}
	//B
	x = -lenth;
	for (int i = 0; i < 3; i++)
	{
		z = lenth - 0.6667*lenth*i;
		for (int j = 0; j < 3; j++)
		{
			y = lenth - j*0.6667*lenth;
			wordCoords.push_back(Point3d(x, y, z));
			wordCoords.push_back(Point3d(x, y, z - 0.6667*lenth));
			wordCoords.push_back(Point3d(x, y - 0.6667*lenth, z - 0.6667*lenth));
			wordCoords.push_back(Point3d(x, y - 0.6667*lenth, z));
		}
	}
	//L
	y = -lenth;
	for (int i = 0; i < 3; i++)
	{
		z = lenth - 0.6667*lenth*i;
		for (int j = 0; j < 3; j++)
		{
			x = -lenth + j*0.6667*lenth;
			wordCoords.push_back(Point3d(x, y, z));
			wordCoords.push_back(Point3d(x, y, z - 0.6667*lenth));
			wordCoords.push_back(Point3d(x + 0.6667*lenth, y, z - 0.6667*lenth));
			wordCoords.push_back(Point3d(x + 0.6667*lenth, y, z));
		}
	}
	//R
	y = lenth;
	for (int i = 0; i < 3; i++)
	{
		z = lenth - 0.6667*lenth*i;
		for (int j = 0; j < 3; j++)
		{
			x = lenth - j*0.6667*lenth;
			wordCoords.push_back(Point3d(x, y, z));
			wordCoords.push_back(Point3d(x, y, z - 0.6667*lenth));
			wordCoords.push_back(Point3d(x - 0.6667*lenth, y, z - 0.6667*lenth));
			wordCoords.push_back(Point3d(x - 0.6667*lenth, y, z));
		}
	}
	//U
	z = lenth;
	for (int i = 0; i < 3; i++)
	{
		x = -lenth + i*0.6667*lenth;
		for (int j = 0; j < 3; j++)
		{
			y = -lenth + 0.6667*lenth*j;
			wordCoords.push_back(Point3d(x, y, z));
			wordCoords.push_back(Point3d(x + 0.6667*lenth, y, z));
			wordCoords.push_back(Point3d(x + 0.6667*lenth, y + 0.6667*lenth, z));
			wordCoords.push_back(Point3d(x, y + 0.6667*lenth, z));
		}
	}
	//D
	z = -lenth;
	for (int i = 0; i < 3; i++)
	{
		x = lenth - i*0.6667*lenth;
		for (int j = 0; j < 3; j++)
		{
			y = -lenth + 0.6667*lenth*j;
			wordCoords.push_back(Point3d(x, y, z));
			wordCoords.push_back(Point3d(x - 0.6667*lenth, y, z));
			wordCoords.push_back(Point3d(x - 0.6667*lenth, y + 0.6667*lenth, z));
			wordCoords.push_back(Point3d(x, y + 0.6667*lenth, z));
		}
	}
	FaceColor colors[] = { Green,Blue,White,Yellow,Red,Orange };

	for (int i = 0; i < 6; i++)	//6个面
	{
		FaceColor clr = colors[i];
		for (int j = 0; j < 9; j++)		//9个小块
		{
			for (int k = 0; k < 4; k++)		//4个顶点
			{
				faces[i * 9 + j].push_back((i * 9 + j) * 4 + k);
			}
			faces[i * 9 + j].push_back(clr);
		}
	}

	D = { 6,7,8,33,34,35,17,16,15,26,25,24,45,46,47,48,49,50,51,52,53 };
	//U 40-55
	U = { 9,10,11,27,28,29,0,1,2,18,19,20,36,37,38,39,40,41,42,43,44 };
	//L
	L = { 36,39,42,0,3,6,51,48,45,11,14,17,18,19,20,21,22,23,24,25,26 };
	//R
	R = { 44,41,38,9,12,15,47,50,53,2,5,8,27,28,29,30,31,32,33,34,35 };
	//F
	F = { 42,43,44,27,30,33,45,46,47,20,23,26,0,1,2,3,4,5,6,7,8 };
	//B
	B = { 38,37,36,18,21,24,53,52,51,29,32,35,9,10,11,12,13,14,15,16,17 };

}
void Cube::updateFlipSet(char face, int direction)
{
	switch (face)
	{
	case 'F':
	{
		swap(0, 2, 8, 6, -direction); swap(1, 5, 7, 3, -direction);
		swap(27, 47, 26, 42, -direction); swap(30, 46, 23, 43, -direction); swap(33, 45, 20, 44, -direction);
	}
	break;
	case 'B':
	{
		swap(9, 11, 17, 15, direction); swap(10, 14, 16, 12, direction);
		swap(38, 18, 51, 35, direction); swap(37, 21, 52, 32, direction); swap(36, 24, 53, 29, direction);
	}
	break;
	case 'L':
	{
		swap(18, 20, 26, 24, -direction); swap(19, 23, 25, 21, -direction);
		swap(36, 0, 45, 17, -direction); swap(39, 3, 48, 14, -direction); swap(42, 6, 51, 11, -direction);
	}
	break;
	case 'R':
	{
		swap(27, 29, 35, 33, direction); swap(28, 32, 34, 30, direction);
		swap(44, 9, 53, 8, direction); swap(41, 12, 50, 5, direction); swap(38, 15, 47, 2, direction);

	}
	break;
	case 'U':
	{
		swap(36, 38, 44, 42, -direction); swap(37, 41, 43, 39, -direction);
		swap(11, 29, 2, 20, -direction); swap(10, 28, 1, 19, -direction); swap(9, 27, 0, 18, -direction);
	}
	break;
	case 'D':
	{
		swap(45, 47, 53, 51, direction); swap(46, 50, 52, 48, direction);
		swap(6, 33, 15, 24, direction); swap(7, 34, 16, 25, direction); swap(8, 35, 17, 26, direction);
	}
	break;
	default:
		break;
	}
}
const std::vector<int>& Cube::getOrder()
{
	// TODO: 在此处插入 return 语句
	for (int i = 0; i < 54; i++)
		drawOrder[i] = i;

	std::sort(drawOrder.begin(), drawOrder.end(),(*greater));
	return drawOrder;
}
void Cube::update()
{
	float costh = (float)std::cos(th);
	float sinth = (float)std::sin(th);
	float cosph = (float)std::cos(ph);
	float sinph = (float)std::sin(ph);
	rh = 2 * std::sqrt(12);			//先不改
	v11 = -sinth; v12 = -cosph*costh; v13 = sinph * costh;
	v21 = costh; v22 = -cosph * sinth; v23 = sinph * sinth;
	v32 = sinph; v33 = cosph;
	v43 = -rh;
}
Cube3d::Cube3d(QWidget * parent /* = 0 */) :QWidget(parent)
{
	setFixedSize(300, 400);
	w = 300, h = 300;
	painter = new QPainter(this);
	flipButton = new QPushButton(tr("Flip"), this);
	connect(flipButton, SIGNAL(clicked()), this, SLOT(FlipForDegbug()));

	cube3dCaptureColorButton = new QPushButton(tr("Capture"), this);
	connect(cube3dCaptureColorButton, SIGNAL(clicked()), this, SLOT(CaptureColorStart()));
	cube3dCaptureColorButton->setGeometry(0, 30, 50, 25);

	operations = new QLineEdit(this);
	operations->setGeometry(100, 0, 250, 20);

	double PI = 3.1415926535;
	cube = new Cube(QSize(300, 800));

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(setCubeColor()));
	leftPressing = false;

	prevMousePos = QPoint(w / 2, h / 2);
	cube->toScreenCoord();
	update();
}
void Cube3d::flip()
{
	flipButton->setEnabled(false);
	std::string seq1 = (operations->text()).toStdString();
	//QRegExp rx(QString("[FBLRUD][']*[0-9]*"));
	//qDebug() << seq1.c_str();
	
	for (int i = 0; i < seq1.size(); i++)
	{
		switch ((char)seq1[i])
		{
		case 'U':
			FlipU();
			break;
		case 'D':
			FlipD();
			break;
		case 'L':
			FlipL();
			break;
		case 'R':
			FlipR();
			break;
		case 'F':
			FlipF();
			break;
		case 'B':
			FlipB();
			break;
		case 'u':
			Flipu();
			break;
		case 'd':
			Flipd();
			break;
		case 'l':
			Flipl();
			break;
		case 'r':
			Flipr();
			break;
		case 'f':
			Flipf();
			break;
		case 'b':
			Flipb();
			break;
		default:
			break;
		}
	}
	((MainWindow *)parentWidget())->setTextEditContent(tr("scrambing finished!\n solving"));
	std::vector<int> faceColors(54);
	for (int i = 0; i < 54; i++)
		faceColors[i] = cube->faces[i][4];
	flipButton->setEnabled(true);
}
void Cube3d::setCubeColor()
{
	cube->setColor(((MainWindow *)parentWidget())->getCubeColor());
	update();
}
void Cube3d::CaptureColorStart()
{
	//timer->start();
	setCubeColor();
}
QString Cube3d::SimplizeResult(const QString &result)
{
    int cnt = 0;
    QChar prev='-';
    QString simplized;
    for(int i=0;i<result.size();i++)
    {
        if(prev == result[i])       //如果和前一个相等，统计相同项的个数
        {
            cnt++;
        }
        else
        {
            if(cnt == 3)            //如果有三个相等的，那么相当于一个反转
            {
                simplized += prev.toLower();
            }
            else{
                for(int j=0;j<cnt;j++)
                {
                    simplized+=prev;
                }
            }
            prev = result[i];
            cnt = 1;
        }
        if(i >= result.size() - 1)      //如果当前是最后一个，那么把当前统计的进行输出
        {
            if(cnt == 3)
            {
                simplized += prev.toLower();
            }
            else{
                for(int j=0;j<cnt;j++)
                {
                    simplized+=prev;
                }
            }
        }
    }
    return simplized;
}
void Cube3d::CaptureColorStop()
{
	//timer->stop();
}
void Cube3d::paintEvent(QPaintEvent *event)
{
	painter->begin(this);
	painter->setPen(Qt::SolidLine);
	QPolygonF polygon;
	polygon.resize(4);
	const std::vector<int> & drawOrder = cube->getOrder();

	for (int k = 0; k < 54; k++)	//绘制54个面
	{
		int i = drawOrder[k];	//按已经计算好的Z深度绘制
        int p1 = cube->faces[i][0], p2 = cube->faces[i][1], p3 = cube->faces[i][2];
		const QPointF & a = cube->screenCoords[p1], &b = cube->screenCoords[p2], &c = cube->screenCoords[p3];
		//可见面判别
		//if ((a.x() - c.x())*(b.y() - c.y()) - (a.y() - c.y())*(b.x() - c.x()) > 0)
		{
			for (int j = 0; j < 4; j++)
			{
				int index = cube->faces[i][j];
				polygon[j] =
					QPointF(
						iX((cube->screenCoords)[index].x()),
						iY((cube->screenCoords)[index].y())
					);
			}
			switch (cube->faces[i][4])
			{
			case FaceColor::Blue:
				painter->setBrush(QColor(0, 0, 255));
				break;
			case FaceColor::Red:
				painter->setBrush(QColor(255, 0, 0));
				break;
			case  FaceColor::Green:
				painter->setBrush(QColor(0, 204, 0));
				break;
			case  FaceColor::White:
				painter->setBrush(QColor(255, 255, 255));
				break;
			case  FaceColor::Yellow:
				painter->setBrush(QColor(255, 215, 0));
				break;
			case FaceColor::Orange:
				painter->setBrush(QColor(250, 128, 10));
				break;
			case -1:
				painter->setBrush(QColor(0,0,0));
				break;
			default:
				break;
			}
			painter->drawPolygon(polygon);
		}
	}

	//((MainWindow *)parentWidget())->setTextEditContent(tr("%1 %2").arg(cube->pointsScreenCoords[0].x()).arg(cube->pointsScreenCoords[0].y()));
	painter->end();
}
void Cube3d::mousePressEvent(QMouseEvent *event)
{
	//QMessageBox::warning(this, tr("pos"), tr("%1 %2").arg(event->pos().x()).arg(event->pos().y()));
	if (event->button() == Qt::LeftButton)
	{
		leftPressing = true;
		prevMousePos = event->pos();
	}
}
void Cube3d::mouseReleaseEvent(QMouseEvent *event)
{
	if (leftPressing == true)
	{
		leftPressing = false;
	}
}
void Cube3d::mouseMoveEvent(QMouseEvent *event)
{
	//视角
	if (leftPressing == true)
	{
		QPoint curMousePos = event->pos();
		int absHorizontal = abs(curMousePos.x() - prevMousePos.x());
		int absVertical = abs(curMousePos.y() - prevMousePos.y());
		if (absHorizontal > absVertical)
		{
			if (curMousePos.x() > prevMousePos.x())
				cube->addTheta(-0.02);
			else if (curMousePos.x() < prevMousePos.x())
				cube->addTheta(0.02);
		}
		else if (absHorizontal < absVertical)
		{

			if (curMousePos.y() > prevMousePos.y())
				cube->addPhi(-0.02);
			else if (curMousePos.y() < prevMousePos.y())
				cube->addPhi(+0.02);
		}
		prevMousePos = event->pos();
		update();
	}
}
void Cube3d::wheelEvent(QWheelEvent *event)
{
	//滚轮控制视野远近
	if (event->delta() > 0)
	{
		cube->addDistance(15);
	}
	else
	{
		cube->addDistance(-15);
	}
	update();
}
Cube3d::~Cube3d()
{
	delete cube;
}

