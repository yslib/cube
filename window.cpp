
#include "window.h"
#include "QtMultimedia/QCameraInfo"
#include "QFile"

#include "QList"
MainWindow::MainWindow(QWidget * parent /* = 0 */) :QWidget(parent)
{

	setAutoFillBackground(true);
    
	QPalette pal = (this->palette());

	/*魔方描述表*/
	cubePosition = new CubePosition();
    
	//摄像头
	//DetectAreaIndex detectArea1 = {0,1,2,5,8,42,43,44,27,30,33,28,29,41,38,3,4,6,7,36,38,40,39,40,32,33,35,36,31,34,37};
	//DetectAreaIndex detectArea2 = {18,19,20,21,22,23,24,25,26,9,10,11,12,13,14,15,16,17,45,46,47,48,49,50,51,52,53 };
	
    //打开记录信息文件，包括采集点索引，立方体的三个参数theta,phi,rho
    //QArrayData data;
    
    /*available cameras*/
    QList<QCameraInfo>  vList =  QList<QCameraInfo>(QCameraInfo::availableCameras());
    std::vector<QCameraInfo *> vPtrList(5,NULL);
    for(int i=0;i<vList.size();i++)
    {
        vPtrList[i]=&vList[i];
    }
    CaputreFrameCubeConfig config;
    std::vector<DetectAreaIndex> indexes(4);
    if(readConfig(0, &config) == false)
        capture0 = new Video((vPtrList)[0], this, 0, cubePosition);
    else{
        capture0 = new Video((vPtrList)[0], this, 0,cubePosition, &config.detectAreaIndex,config.theta,config.phi,config.rho);
        indexes[0]=config.detectAreaIndex;
    }
    if(readConfig(1, &config) == false)
        capture1 = new Video((vPtrList)[1], this, 1, cubePosition);
    else{
        capture1 = new Video((vPtrList)[1], this, 1, cubePosition,&config.detectAreaIndex,config.theta,config.phi,config.rho);
        indexes[1]=config.detectAreaIndex;
    }
    if(readConfig(2, &config) == false)
         capture2 = new Video((vPtrList)[2], this, 2, cubePosition);
    else{
        capture2 = new Video((vPtrList)[2], this, 2, cubePosition,&config.detectAreaIndex,config.theta,config.phi,config.rho);
        indexes[2]=config.detectAreaIndex;
    }
    if(readConfig(4, &config) == false)
        capture3 = new Video((vPtrList)[4], this, 4, cubePosition);
    else{
        capture3 = new Video((vPtrList)[4], this, 4, cubePosition,&config.detectAreaIndex,config.theta,config.phi,config.rho);
        indexes[3] = config.detectAreaIndex;
    }
    
	//for (int i = 0; i < vlist.size(); i++)
	//{
	//	*captureList[i] = new Video(vlist[i], this, i, cubePosition);
	//}
	ptrVideos.push_back(capture0);
	ptrVideos.push_back(capture1);
	ptrVideos.push_back(capture2);
	ptrVideos.push_back(capture3);

	capture0->setFixedSize(QSize(340, 280));
	capture1->setFixedSize(QSize(340, 280));
	capture2->setFixedSize(QSize(340, 280));
	capture3->setFixedSize(QSize(340, 280));
	//information ouput textedit
	outputText = new QTextEdit(this);
	outputText->setFixedSize(QSize(350, 280));
	outputText->setReadOnly(true);
	//2d cube figure
	cube2d = new Cube2d(ptrVideos, this);
	cube2d->setFixedSize(QSize(400, 300));
    for(int i=0;i<4;i++)
    {
        for(size_t j=0;j<indexes[i].size();j++)
        {
            cube2d->getDetectAreaIndex(i)->push_back(indexes[i][j]);
            cube2d->getSelectedDetectPoints().insert(indexes[i][j]);
        }
    }
	//3d cube figure
	cube3d = new Cube3d(this);
	cube3d->setFixedSize(QSize(400, 300));
	//button
	solveButton = new QPushButton(tr("Solve"), this);
	exitButton = new QPushButton(tr("Quit"), this);
    //signal connections
	connect(solveButton, SIGNAL(clicked()), this, SLOT(solve()));
	connect(exitButton, SIGNAL(clicked()),this, SLOT(exit_from_window()));

	serialPort = new QSerialPort(this);
	serialLabel = new QLabel(tr("SerialPorts:"));
    /*available serial ports*/
	serialPortCombBox = new QComboBox(this);
	connect(serialPortCombBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setSerialPort(const QString &)));
    foreach (const QSerialPortInfo & info , QSerialPortInfo::availablePorts())
	{
		serialPortCombBox->addItem(info.portName());
	}
	/*layout*/
	QHBoxLayout * videosLayout = new QHBoxLayout();
	videosLayout->addWidget(capture0);
	videosLayout->addWidget(capture1);
	videosLayout->addWidget(capture2);
	videosLayout->addWidget(capture3);

	QHBoxLayout * buttonsLayout = new QHBoxLayout();
	buttonsLayout->addWidget(solveButton);
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(serialLabel);
	buttonsLayout->addWidget(serialPortCombBox);
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(exitButton);

	QVBoxLayout * bottomMiddleLayout = new QVBoxLayout();
	bottomMiddleLayout->addWidget(cube2d);
	bottomMiddleLayout->addLayout(buttonsLayout);

	QHBoxLayout * bottomLayout = new QHBoxLayout();

	bottomLayout->addWidget(outputText);
	bottomLayout->addLayout(bottomMiddleLayout);

	QVBoxLayout * cube3dLayout = new QVBoxLayout();
	cube3dLayout->addWidget(cube3d);

	bottomLayout->addLayout(cube3dLayout);
	QVBoxLayout * mainLayout = new QVBoxLayout();
	mainLayout->addLayout(videosLayout);
	mainLayout->addLayout(bottomLayout);
	setLayout(mainLayout);
	setFixedSize(QSize(1400, 600));
	setWindowTitle(tr("Cube Solver"));
}

void MainWindow::initVideos()
{

}

bool MainWindow::readConfig(int videoNumber,CaputreFrameCubeConfig * config)
{
    QString prefix="frame";
    QString fileName = prefix+QChar(char(videoNumber+'0'));
    QFile file(fileName);
    
    if(file.open(QIODevice::ReadOnly) == false)
        return false;
    config->detectAreaIndex.resize(0);
    QDataStream in(&file);
    quint32 magicNumber;
    in>>magicNumber;
    if(magicNumber != MagicNumber)
        return false;
    in>>config->theta>>config->phi>>config->rho;
    while(in.atEnd() == false)
    {
        qint32 index;
        in>>index;
        config->detectAreaIndex.push_back(index);
    }
    file.close();
    return true;
}

void MainWindow::setSerialPort(const QString & text)
{
	
	if (serialPort->isOpen() == true)
		serialPort->close();
	serialPort->setPortName(text);
	serialPort->setBaudRate(115200);
	serialPort->setDataBits(QSerialPort::Data8);
	serialPort->setParity(QSerialPort::NoParity);
	serialPort->setStopBits(QSerialPort::OneStop);
	serialPort->setFlowControl(QSerialPort::NoFlowControl);
	serialPort->open(QIODevice::WriteOnly);
}
void MainWindow::appendTextEditContent(const QString & text)
{
	outputText->append(text);
}
void MainWindow::setTextEditContent(const QString & text)
{
	outputText->setText(text);
}
void MainWindow::solve()
{
	//通过得到的颜色信息转换为算法的输入
	CubeInput cubeInput(cube2d->getCubeColor());
	CubeSolver cubeSolver;
	cubeSolver.SetInput(cubeInput);

	std::vector<char> movement;
	std::vector<int> moveamount;
	cubeSolver.Solve(movement, moveamount);
	QString results;
	std::string seq;
	for (int i = 0; i < movement.size(); i++)
	{
		results += movement[i];
		results += char(moveamount[i] + '0');
		for (int j = 0; j < moveamount[i]; j++)
			seq += movement[i];
	}
	setTextEditContent(results);
	serialPort->write(results.toLatin1().data());
	//3d flip
	cube3d->flip();
}
void MainWindow::exit_from_window() {
    delete capture0;
    delete capture1;
    delete capture2;
    delete capture3;
    delete cube2d;
    delete cube3d;
    delete cubePosition;
	qApp->quit();
}
