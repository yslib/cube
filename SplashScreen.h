#pragma once
#include "all.h"
#include <QDialog>

class SplashScreen :public QDialog
{
	Q_OBJECT
private:
	QLabel * imageLabel;
	QPixmap * image;
public:
	SplashScreen(QWidget * parent = 0);
};