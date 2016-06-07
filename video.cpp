#include "video.h"
#include "window.h"

Video::Video(QCameraInfo * cameraInfo,QWidget * parent, int dev, CubePosition * cubePosition, DetectAreaIndex * detectIndex,double theta,double phi,double rho) :QWidget(parent)
{
	device = dev;   //设备号

    width = 320;	//图像默认大小

    height = 240;

	imageLabel = new QLabel(this);      //输显示到Label

    //image = new QImage(width,height,QImage::Format_RGB888);

	this->cubePosition = cubePosition;

	//painter = new QPainter();

	capFrame = new CaptureCubeFrame(cubePosition,detectIndex,QSize(width, height),theta,phi,rho);

	//frame = new cv::Mat;

	//cap = new cv::VideoCapture;

    //finder = new QCameraViewfinder(this);
    
    if(cameraInfo != NULL)
    {
        videoSurface = new VideoSurface(imageLabel,NULL, capFrame, detectIndex, cubePosition);
        
        camera = new QCamera(*cameraInfo,this);
        
        camera->setCaptureMode(QCamera::CaptureVideo);
        
        camera->setViewfinder(videoSurface);
        
        QCameraViewfinderSettings settings = camera->viewfinderSettings();
        
        settings.setResolution(QSize(width, height));
        
        camera->setViewfinderSettings(settings);
    }
    else
    {
        camera = NULL;
    }
    /*set background*/
    QImage backGround("bg.png");
    imageLabel->setPixmap(QPixmap::fromImage(backGround.scaled(width,height)));
    
    //camera->start();
    
    //QSize resolution = camera->viewfinderSettings().resolution();
    //qDebug()<<resolution;

//    QList<QSize> supportedResolutions = camera->supportedViewfinderResolutions();
//    foreach(const QSize & each,supportedResolutions)
//    {
//        qDebug()<<each;
//    }

//    capThread = new CaptureThread(width, height, cap,
//        frame, painter, image,
//        imageLabel,
//        cubePosition,
//        detectIndex,
//        capFrame,device,this);

//    timer = new QTimer(this);
//    connect(timer,SIGNAL(timeout()),this,SLOT(capture()));

	startCameraButton = new QPushButton(tr("Start"), this);
	connect(startCameraButton, SIGNAL(clicked()), this, SLOT(openCamera()));

	stopCameraButton = new QPushButton(tr("Stop"), this);
	connect(stopCameraButton, SIGNAL(clicked()), this, SLOT(closeCamera()));

	leftPressing = false;
	rightPressing = false;

	/*布局*/
	QHBoxLayout * buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(startCameraButton);
	buttonLayout->addWidget(stopCameraButton);
	QVBoxLayout * mainLayout = new QVBoxLayout;
	mainLayout->addWidget(imageLabel);
	mainLayout->addLayout(buttonLayout);
	setLayout(mainLayout);
}
void Video::readConfigure()
{
    
}
bool Video::writeConfigure(int videoNumber,CaputreFrameCubeConfig * config)
{
    QString prefix="frame";
    QString fileName = prefix+QChar(char(videoNumber+'0'));
    QFile file(fileName);
    
    if(file.open(QIODevice::WriteOnly) == false)
        return false;
    file.resize(0);
    QDataStream out(&file);
    out<<(quint32)MagicNumber;
    out<<config->theta<<config->phi<<config->rho;
    for(size_t i=0;i<config->detectAreaIndex.size();i++)
        out<<qint32(config->detectAreaIndex[i]);
    file.close();
    return true;
}
void Video::openCamera()
{
    /*multiple thread*/
//    if (cap->isOpened() == true)
//        return;
//    cap->open(device);
//    if (cap->isOpened() == true)
//    {
//        cap->set(CV_CAP_PROP_FRAME_WIDTH, width);
//        cap->set(CV_CAP_PROP_FRAME_WIDTH, height);
//        capThread->start();
//    }
//    else
//       QMessageBox::warning(this, tr("Error"), tr("Cannot open video"));
    /*QCamera*/
    if (camera == NULL) {
        QMessageBox::warning(this, tr("Error"), tr("Camera was not loaded!"));
        return;
    }
        if (camera->state() == QCamera::ActiveState)
        {
            QMessageBox::warning(this, tr("Error"), tr("Cannot open video"));
            return;
        }
        camera->start();
        if (camera->state() != QCamera::ActiveState)
        {
            QMessageBox::warning(this, tr("Error"), tr("Cannot open video"));
        }

    /*Single Camera*/
//    if (cap->isOpened() == true)
//        return;
//    cap->open(device);
//    if (cap->isOpened() == true)
//    {
//        cap->set(CV_CAP_PROP_FRAME_WIDTH, width);
//        cap->set(CV_CAP_PROP_FRAME_WIDTH, height);
//        timer->start(50);
//    }
//    else
//        QMessageBox::warning(this, tr("Error"), tr("Cannot open video"));

}
void Video::closeCamera()
{
    if(camera == NULL)
    {
        QMessageBox::warning(this, tr("Error"), tr("Camera was not loaded!"));
        return ;
    }
    /*multiple thread*/
//  capThread->stop();
//  cap->release();
    /*QCamera*/
    camera->stop();
    /*Single Camera*/
//  timer->stop();
//  cap->release();
    QImage backGround("bg.png");
    imageLabel->setPixmap(QPixmap::fromImage(backGround.scaled(width,height)));
}

void Video::somethingError()
{
	QMessageBox::warning(this, tr("Error"),tr("Error"));
}

void Video::dLine(int p1, int p2)
{
	float x1 = capFrame->screenCoords[p1].x();
	float y1 = capFrame->screenCoords[p1].y();
	float x2 = capFrame->screenCoords[p2].x();
	float y2 = capFrame->screenCoords[p2].y();
	painter->drawLine(iX(x1), iY(y1), iX(x2), iY(y2));
}
void Video::capture()
{
	*cap >> *frame;

    if(frame->data == NULL)
        return;
	cv::resize(*frame, *frame, cv::Size(width, height));

	//根据采集点的位置采集颜色
	for (int i = 0; i < capFrame->toIndex.size(); i++)
	{
		int centre_x = (capFrame->detectPointsScreenCoords)[i].x();
		int centre_y = (capFrame->detectPointsScreenCoords)[i].y();
		if(centre_x <0 || centre_x> width || centre_y < 0 || centre_y>height)
			continue;
		uchar * b = frame->data
			+ centre_y * (frame->cols*frame->channels() * 1)
			+ centre_x * (frame->channels() * 1);
		uchar * g = b + 1;
		uchar * r = g + 1;
		(cubePosition->rgbs)[capFrame->toIndex[i]] = sRGB(*r,*g, *b);
	}
	//转换成QImage
	cv::cvtColor(*frame, *frame, CV_BGR2RGB);

	*image = QImage(frame->data,
		frame->cols, frame->rows,
		frame->cols*frame->channels(),
        QImage::Format_RGB888).copy();

	//绘制边
	painter->begin(image);
	painter->setPen(Qt::white);
	dLine(0, 1); dLine(0, 2); dLine(3, 2); dLine(3, 1);
	dLine(0, 4); dLine(2, 6); dLine(3, 7); dLine(1, 5);
	dLine(4, 5); dLine(5, 7); dLine(7, 6); dLine(6, 4);
	//绘制采集点
	painter->drawPoints(capFrame->detectPointsScreenCoords);
	//((MainWindow *)parentWidget())->setTextEditContent(tr("%1").arg(capFrame->detectPointsScreenCoords.size()));
	painter->end();
	imageLabel->setPixmap(QPixmap::fromImage(*image));
    imageLabel->update();
}
void Video::setDetectPoints(const DetectAreaIndex * detectIndex)
{
	capFrame->setDetectPoints(detectIndex);
}
void Video::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		leftPressing = true;
	}
	if (event->button() == Qt::RightButton)
	{
		rightPressing = true;
	}
	prevMousePos = event->pos();
}
void Video::mouseReleaseEvent(QMouseEvent *event)
{
	leftPressing = false;
	rightPressing = false;
}
void Video::mouseMoveEvent(QMouseEvent *event)
{
	//拖动左键改变视角

	if (leftPressing == true)
	{
		QPoint curMousePos = event->pos();
		int absHorizontal = abs(curMousePos.x() - prevMousePos.x());
		int absVertical = abs(curMousePos.y() - prevMousePos.y());
		if (absHorizontal > absVertical)
		{
			if (curMousePos.x() > prevMousePos.x())
				capFrame->addTheta(-0.1);
			else if (curMousePos.x() < prevMousePos.x())
				capFrame->addTheta(0.1);
		}
		else if (absHorizontal < absVertical)
		{

			if (curMousePos.y() > prevMousePos.y())
				capFrame->addPhi(-0.1);
			else if (curMousePos.y() < prevMousePos.y())
				capFrame->addPhi(+0.1);
		}
		prevMousePos = event->pos();
		
	}
	else if (rightPressing == true)		//拖动右键改变透视程度
	{
		QPoint curMousePos = event->pos();
		if (curMousePos.y() > prevMousePos.y())
			capFrame->addRho(-0.1);
		else if (curMousePos.y() < prevMousePos.y())
			capFrame->addRho(+0.1);
	}
}
void Video::wheelEvent(QWheelEvent *event)
{
	//滚轮控制视野远近
	if (event->delta() > 0)
	{
		capFrame->addDistance(15);
	}
	else
	{
		capFrame->addDistance(-15);
	}
	update();
}
Video::~Video()
{
    CaputreFrameCubeConfig config;
    config.theta = capFrame->getTheta();
    config.phi = capFrame->getPhi();
    config.rho = capFrame->getRho();
    for(int i=0;i<capFrame->toIndex.size();i++)
    {
        config.detectAreaIndex.push_back(capFrame->toIndex[i]);
    }
    writeConfigure(device,&config);
}
/************************************************************************/
/* CaptureCubeFrame                                                     */
/************************************************************************/
//const double CaptureCubeFrame::PI = 3.1415926535;
CaptureCubeFrame::CaptureCubeFrame(CubePosition * cubePosition, DetectAreaIndex * detectIndex, QSize sceenSize, double theta /* = PI/4 */, double phi /* = PI/4 */, double rho /* = 10.00 */)
{
	cubeInfo = cubePosition;
	th = theta;
	ph = phi;
	rh = 2 * std::sqrt(12);		//没有采用默认值，因为这个值使透视效果看起来最好
	this->screenSize = sceenSize;
	update();
    
	wordCoords.resize(8);
	screenCoords.resize(8);
	float lenth = 1;
	wordCoords[0] = Point3d(lenth, lenth, lenth);
	wordCoords[1] = Point3d(lenth, lenth, -lenth);;
	wordCoords[2] = Point3d(lenth, -lenth, lenth);
	wordCoords[3] = Point3d(lenth, -lenth, -lenth);
	wordCoords[4] = Point3d(-lenth, lenth, lenth);
	wordCoords[5] = Point3d(-lenth, lenth, -lenth);
	wordCoords[6] = Point3d(-lenth, -lenth, lenth);
	wordCoords[7] = Point3d(-lenth, -lenth, -lenth);
	setDetectPoints(detectIndex);
}
void CaptureCubeFrame::toScreenCoord()
{
	//顶点坐标的世界坐标到屏幕坐标的转换
	for (int i = 0; i < 8; i++)
	{
		Point3d & w = wordCoords[i];
		float x = v11*w.x + v21*w.y;
		float y = v12*w.x + v22*w.y + v32*w.z;
		float z = v13*w.x + v23*w.y + v33*w.z + v43;
		screenCoords[i] = QPointF(-d*x / z, -d*y / z);
	}
	//检测区域坐标区域的世界坐标到屏幕坐标的转换
	for (int i = 0; i < detectPointsWorldCoords.size(); i++)
	{
		Point3d & w = detectPointsWorldCoords[i];
		float x = v11*w.x + v21*w.y;
		float y = v12*w.x + v22*w.y + v32*w.z;
		float z = v13*w.x + v23*w.y + v33*w.z + v43;
		detectPointsScreenCoords[i] = QPoint(screenSize.width() / 2 - d*x / z, screenSize.height() / 2 + d*y / z);
	}
	
}
void CaptureCubeFrame::update()
{
	//
	float costh = (float)std::cos(th);
	float sinth = (float)std::sin(th);
	float cosph = (float)std::cos(ph);
	float sinph = (float)std::sin(ph);
	//rh = 2 * std::sqrt(12);

	v11 = -sinth; v12 = -cosph*costh; v13 = sinph * costh;
	v21 = costh; v22 = -cosph * sinth; v23 = sinph * sinth;
	v32 = sinph; v33 = cosph;
	v43 = -rh;
	d = rh / 1.5 * (screenSize.width() < screenSize.height() ? screenSize.width() : screenSize.height()) / std::sqrt(12);
}
void CaptureCubeFrame::setTheta(float theta)
{
	th = theta; 
	update(); 
	toScreenCoord();
}
void CaptureCubeFrame::setPhi(float phi)
{
	ph = phi;
	update();
	toScreenCoord();
}
void CaptureCubeFrame::setRho(float rho)
{
	rh = rho; 
	v43 = -rh;
	toScreenCoord();
}
float CaptureCubeFrame::getPhi()
{
    return ph;
}
float CaptureCubeFrame::getTheta()
{
    return th;
}
float CaptureCubeFrame::getRho()
{
    return rh;
}
float CaptureCubeFrame::getDistance()
{
    return d;
}
void CaptureCubeFrame::setDetectPoints(const DetectAreaIndex * detectIndex)
{
	//设置采集点的世界坐标和屏幕坐标
	if (detectIndex != NULL && cubeInfo != NULL)
	{
		detectPointsWorldCoords.resize(detectIndex->size());
		detectPointsScreenCoords.resize(detectIndex->size());
		toIndex.resize(detectIndex->size());		//第i个坐标对应的编号

		QSet<int> a;
		QSet<int>::iterator itr;
		for (int i = 0; i < detectIndex->size(); i++)
		{
			detectPointsWorldCoords[i] = (cubeInfo->getAreaMapping())[(*detectIndex)[i]];
            //You need this index to retrive
			toIndex[i] = (*detectIndex)[i];
		}
	}
	toScreenCoord();
}

CaptureThread::CaptureThread(
	int w,int h,
	cv::VideoCapture * cp,
	cv::Mat * frm,
	QPainter * pter, 
	QImage * img, 
	QLabel * lbl,
	CubePosition * cb, 
	DetectAreaIndex * da,
    CaptureCubeFrame * cf,int dev,Video* vd)
{
    image = new QImage(320,240,QImage::Format_RGB888);
	imageLabel = lbl;
	cubePosition = cb;
	detectArea = da;
	capFrame = cf;
	cap = cp;
	frame = frm;
    painter = new QPainter();
	width = w;
	height = h;
    device=dev;
	stopped = false;
    video =vd;
}

void CaptureThread::run()
{
    while (true)
	{
        mutex.lock();
        if(stopped == true)
        {
            stopped =false;
            mutex.unlock();
            break;
        }
        mutex.unlock();
        cv::Mat srcFrame;
        *cap>>srcFrame;
        if (srcFrame.data == NULL)
        {
            qDebug()<<"NULL";
            continue;
        }
        cv::resize(srcFrame, *frame, cv::Size(width, height));
		//根据采集点的位置采集颜色
		for (int i = 0; i < capFrame->toIndex.size(); i++)
		{
			int centre_x = (capFrame->detectPointsScreenCoords)[i].x();
			int centre_y = (capFrame->detectPointsScreenCoords)[i].y();
			if (centre_x <0 || centre_x> width || centre_y < 0 || centre_y>height)
				continue;
			uchar * b = frame->data
				+ centre_y * (frame->cols*frame->channels() * 1)
				+ centre_x * (frame->channels() * 1);
			uchar * g = b + 1;
			uchar * r = g + 1;
            
			(cubePosition->rgbs)[capFrame->toIndex[i]] = sRGB(*r, *g, *b);
		}
		//转换成QImage
		cv::cvtColor(*frame, *frame, CV_BGR2RGB);
		*image = QImage(frame->data,
			frame->cols, frame->rows,
			frame->cols*frame->channels(),
            QImage::Format_RGB888).copy();
		//绘制边
        painter->begin(image);
		painter->setPen(Qt::white);
		dLine(0, 1); dLine(0, 2); dLine(3, 2); dLine(3, 1);
		dLine(0, 4); dLine(2, 6); dLine(3, 7); dLine(1, 5);
		dLine(4, 5); dLine(5, 7); dLine(7, 6); dLine(6, 4);
		//绘制采集点
		painter->drawPoints(capFrame->detectPointsScreenCoords);
        painter->end();
//        painterVideo->begin(imageLabel);
//        painterVideo->drawImage(QPoint(0,0),*image,QRect(0,0,320,240));
//        painterVideo->end();
        imageLabel->setPixmap(QPixmap::fromImage(*image));
        imageLabel->update();
        cv::waitKey(200);
	}
}
void CaptureThread::dLine(int p1, int p2)
{
	float x1 = capFrame->screenCoords[p1].x();
	float y1 = capFrame->screenCoords[p1].y();
	float x2 = capFrame->screenCoords[p2].x();
	float y2 = capFrame->screenCoords[p2].y();
	painter->drawLine(iX(x1), iY(y1), iX(x2), iY(y2));
}



VideoSurface::VideoSurface(QLabel *canvas, QPainter * pter, CaptureCubeFrame * cf, DetectAreaIndex * da, CubePosition * cp)
{
    imageLabel = canvas;
    image = new QImage(320, 240,QImage::Format_ARGB32);
    capFrame = cf;
    detectArea = da;
    cubePosition = cp;
    painter = new QPainter();
    width=320;
    height=240;
}
QList<QVideoFrame::PixelFormat>
VideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType)const
{
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_ARGB32;
    }
    return QList<QVideoFrame::PixelFormat>();
}

VideoSurface::~VideoSurface()
{
    delete image;
    delete painter;
}

bool VideoSurface::present(const QVideoFrame &frame)
{

    QVideoFrame frametodraw(frame);
    if (frametodraw.map(QAbstractVideoBuffer::ReadOnly) == false)
    {
        
    }
    *image = QImage(frametodraw.bits(),
        frametodraw.width(),
        frametodraw.height(),
        frametodraw.bytesPerLine(),
        QImage::Format_ARGB32).scaled(320,240);
    /*Process Begin*/
    for (int i = 0; i < capFrame->toIndex.size(); i++)
    {
        int centre_x = (capFrame->detectPointsScreenCoords)[i].x();

        int centre_y = (capFrame->detectPointsScreenCoords)[i].y();

        if (centre_x <0 || centre_x> width || centre_y < 0 || centre_y>height)
            continue;

        const uchar * b = (image->constBits() + centre_y*(image->bytesPerLine()) + centre_x*(image->depth() / 8));
        const uchar * g = b + 1;
        const uchar * r = g + 1;
        (cubePosition->rgbs)[capFrame->toIndex[i]] = sRGB(*r, *g, *b);
    }
    //绘制边
    painter->begin(image);
    painter->setPen(Qt::white);
    dLine(0, 1); dLine(0, 2); dLine(3, 2); dLine(3, 1);
    dLine(0, 4); dLine(2, 6); dLine(3, 7); dLine(1, 5);
    dLine(4, 5); dLine(5, 7); dLine(7, 6); dLine(6, 4);
    //绘制采集点
    painter->drawPoints(capFrame->detectPointsScreenCoords);
    //((MainWindow *)parentWidget())->setTextEditContent(tr("%1").arg(capFrame->detectPointsScreenCoords.size()));
    painter->end();
    
    /*Process End*/
    imageLabel->setPixmap(QPixmap::fromImage(*image));
    imageLabel->update();
    frametodraw.unmap();
    return true;
}

void VideoSurface::dLine(int p1, int p2)
{
    float x1 = capFrame->screenCoords[p1].x();
    float y1 = capFrame->screenCoords[p1].y();
    float x2 = capFrame->screenCoords[p2].x();
    float y2 = capFrame->screenCoords[p2].y();
    painter->drawLine(iX(x1), iY(y1), iX(x2), iY(y2));
}

//VideoSurface::VideoSurface(QLabel *lbl)
//{
//	label = lbl;
//    image = new QImage(320,240, QImage::Format_ARGB32);
//}

//QList<QVideoFrame::PixelFormat>
//VideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType /* = QAbstractVideoBuffer::NoHandle */)const
//{
//	if (handleType == QAbstractVideoBuffer::NoHandle) {
//        return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_ARGB32;
//	}
//	return QList<QVideoFrame::PixelFormat>();
//}
//bool VideoSurface::present(const QVideoFrame &frame)
//{

//	QVideoFrame frametodraw(frame);
//	if (frametodraw.map(QAbstractVideoBuffer::ReadOnly) == false)
//	{
		
//	}
//	*image = QImage(frametodraw.bits(),
//		frametodraw.width(),
//		frametodraw.height(),
//		frametodraw.bytesPerLine(),
//        QImage::Format_ARGB32).copy().scaled(320,240);

////    label->resize(image->size());
////    qDebug()<<image->bits();
////    qDebug()<<image->width();
////    qDebug()<<image->bytesPerLine();
////    qDebug()<<frametodraw.bits();
////    qDebug()<<frametodraw.height();
////    qDebug()<<frametodraw.width();
////    qDebug()<<image->size();
//	label->setPixmap(QPixmap::fromImage(*image));
//    label->repaint();
//	frametodraw.unmap();
//	return true;
//}
