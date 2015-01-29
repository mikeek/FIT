#-------------------------------------------------
#
# Project created by QtCreator 2014-04-22T18:18:23
#
#-------------------------------------------------

include(./icp.pri)

QT       += core gui widgets

CONFIG += qt

TARGET = bludiste2014
TEMPLATE = app

LIBS += -lboost_system

QMAKE_MAKEFILE = QMakefile
QMAKE_CXXFLAGS += -std=c++11 -lboost_system

SOURCES += main.cpp\
        mainwindow.cpp\
        Client.cpp \
        mapmenu.cpp \
    gamewindow.cpp \
    winnerswindow.cpp \
    infolabel.cpp \
    emitter.cpp

HEADERS  += mainwindow.h \
            Client.h \
            mapmenu.h \
    gamewindow.h \
    Message.h \
    winnerswindow.h \
    infolabel.h \
    emitter.h

FORMS    += mainwindow.ui \
    mapmenu.ui \
    gamewindow.ui \
    winnerswindow.ui

RESOURCES += \
    images.qrc
