#-------------------------------------------------
#
# Project created by QtCreator 2017-12-11T11:38:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WorldGeneratorGUI
TEMPLATE = app
INCLUDEPATH += ../WorldGenerator
DEPENDPATH += ../WorldGenerator/
LIBS+=-L../build-WorldGenerator-Desktop-Debug -lWorldGenerator


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
