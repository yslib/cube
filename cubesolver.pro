#-------------------------------------------------
#
# Project created by QtCreator 2016-05-11T13:14:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cubesolver
TEMPLATE = app
INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/include
CONFIG+=c++11
SOURCES += main.cpp\
    cube2d.cpp \
    cube3d.cpp \
    CubeSolver.cpp \
    getcolor.cpp \
    SplashScreen.cpp \
    video.cpp \
    window.cpp

HEADERS  += \
    all.h \
    common.h \
    cube2d.h \
    cube3d.h \
    CubeSolver.h \
    getcolor.h \
    SplashScreen.h \
    video.h \
    window.h

QT += multimedia
QT += multimediawidgets
QT += serialport

LIBS += -L/usr/local/lib -lopencv_video -lopencv_core -lopencv_highgui -lopencv_videostab -lopencv_imgproc -lopencv_legacy -lopencv_videoio
