# tests.pro
#
# The tests/ QMake project fiel for Switchyard
#
# (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of version 2.1 of the GNU Lesser General Public
#    License as published by the Free Software Foundation;
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, 
#    Boston, MA  02111-1307  USA
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
CONFIG += thread
CONFIG += warn_on
CONFIG += release
CONFIG += console

QT += network
