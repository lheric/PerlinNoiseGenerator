#-------------------------------------------------
#
# Project created by QtCreator 2014-08-31T21:13:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PerlinNoiseGenerator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

#include & libs
INCLUDEPATH += .\
               ../libgitlmvc/libgitlevtbus/src \
               ../libgitlmvc/src

LIBS += -L$${OUT_PWD}/../libgitlmvc/libgitlevtbus -L$${OUT_PWD}/../libgitlmvc

CONFIG(debug, debug|release){
    LIBS += -lGitlMVCd -lGitlEvtBusd
}
CONFIG(release, debug|release){
    LIBS += -lGitlMVC  -lGitlEvtBus
}
