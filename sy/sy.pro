# sy.pro
#
# The sy/ QMake project file for Switchyard
#
# (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
#     All Rights Reserved
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
HEADERS += syprofile.h
HEADERS += sysyslog.h
HEADERS += syringbuffer.h
HEADERS += syrouting.h
HEADERS += syrtp_header.h
HEADERS += syrtp_server.h

INCLUDEPATH += ../

LIBS += -lwsock32

CONFIG += qt
CONFIG += dll
CONFIG += thread
CONFIG += release
CONFIG += warn_on

QT += network
