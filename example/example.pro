QT += core
QT -= gui

CONFIG += c++11

TARGET = example
CONFIG += console
CONFIG += debug
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    timeseries.cpp \
    ssaalgo.cpp \
    ftca.cpp

INCLUDEPATH += /usr/include/eigen3/

HEADERS += \
    timeseries.h \
    measurement.h \
    ssaalgo.h \
    ftca.h

DISTFILES +=
