# tests.pro
#
# The tests/ QMake project fiel for Switchyard
#
# (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
#     All Rights Reserved
#

TEMPLATE = app

win32 {
  DEFINES += WIN32
  DEFINES += VERSION=\\\"$$[VERSION]\\\"
  DEFINES += PACKAGE=\\\"switchyard\\\"
}

SOURCES += syd.cpp

HEADERS += syd.h

INCLUDEPATH += ..

LIBS += -L../sy/release -lswitchyard

CONFIG += qt
CONFIG += warn_on
CONFIG += release
CONFIG += windows

QT += network
