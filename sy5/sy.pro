# sy.pro
#
# The sy/ QMake project file for Switchyard
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

TEMPLATE = lib
TARGET = switchyard
win32 {
  DEFINES += WIN32
  DEFINES += VERSION=\\\"$$[VERSION]\\\"
  DEFINES += PACKAGE=\\\"switchyard\\\"
}

SOURCES += syadv_packet.cpp
SOURCES += syadv_reader.cpp
SOURCES += syadv_server.cpp
SOURCES += syadv_source.cpp
SOURCES += syadv_tag.cpp
SOURCES += syastring.cpp
SOURCES += sycmdswitch.cpp
SOURCES += sygpio_server.cpp
SOURCES += sylwrp_clientconnection.cpp
SOURCES += sylwrp_server.cpp
SOURCES += symcastsocket.cpp
SOURCES += synode.cpp
SOURCES += syprofile.cpp
SOURCES += sysyslog.cpp
SOURCES += syringbuffer.cpp
SOURCES += syrouting.cpp
SOURCES += syrtp_header.cpp
SOURCES += syrtp_server.cpp

HEADERS += syadv_packet.h
HEADERS += syadv_reader.h
HEADERS += syadv_server.h
HEADERS += syadv_source.h\
HEADERS += syadv_tag.h
HEADERS += syastring.h
HEADERS += sycmdswitch.h
HEADERS += syconfig.h
HEADERS += sygpio_server.h
HEADERS += sylwrp_clientconnection.h
HEADERS += sylwrp_server.h
HEADERS += symcastsocket.h
HEADERS += synode.h
HEADERS += syprofile.h
HEADERS += sysyslog.h
HEADERS += syringbuffer.h
HEADERS += syrouting.h
HEADERS += syrtp_header.h
HEADERS += syrtp_server.h

INCLUDEPATH += ../

LIBS += -lws2_32 -liphlpapi

CONFIG += qt
CONFIG += dll
CONFIG += thread
CONFIG += release
CONFIG += warn_on

QT += network
