#include "SplashScreen.h"

SplashScreen::SplashScreen(QWidget * parent /* = 0 */) :QDialog(parent)
{
	image = new QPixmap("Splash.png");
	imageLabel = new QLabel(this);
	imageLabel->setPixmap(*image);
	imageLabel->setScaledContents(true);
	setWindowFlags(Qt::FramelessWindowHint);				//去掉标题栏
	setAttribute(Qt::WA_TranslucentBackground, true);		//窗口背景透明
	setFixedSize(500, 500);
}