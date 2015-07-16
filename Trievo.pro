#-------------------------------------------------
#
# Project created by QtCreator 2014-08-06T20:38:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Trievo
TEMPLATE = app

SOURCES += main.cpp\
	mainwindow.cpp \
    sourceimage.cpp

HEADERS  += mainwindow.h \
    sourceimage.h

QMAKE_LFLAGS += -fopenmp
QMAKE_CXXFLAGS += -fopenmp

win32:RC_FILE = myapp.rc

OTHER_FILES += \
    icon.ico \
    myapp.rc
