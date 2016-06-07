
#include <QtWidgets/QApplication>
#include <opencv2/opencv.hpp>
#include <video.h>
#include "window.h"
#include "QSplashScreen"
#include "SplashScreen.h"
#include <algorithm>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow * mainw = new MainWindow(0);

	//SplashScreen * splash = new SplashScreen(0);
	//splash->show();
	//QElapsedTimer t;
	//t.start();
	//while (t.elapsed() < 2000)
	//	QCoreApplication::processEvents();
	//splash->close();
    
	mainw->show();
	return a.exec();
}