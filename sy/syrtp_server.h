// syrtp_server.h
//
// Real Time Protocol
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of version 2.1 of the GNU Lesser General Public
//    License as published by the Free Software Foundation;
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, 
//    Boston, MA  02111-1307  USA
//

#ifndef SYRTP_SERVER_H
#define SYRTP_SERVER_H

#ifdef WIN32
#include <Winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#endif  // WIN32
#include <pthread.h>

#include <QObject>
#include <QTimer>

#include <sy/syrouting.h>

class SyRtpServer : public QObject
{
  Q_OBJECT;
 public:
  SyRtpServer(void *(*callback_func)(uint32_t,const char *,int,SyRouting *,void *),
	      void *callback_priv,SyRouting *routing,QObject *parent=0);
  ~SyRtpServer();
  void shutdown();

 signals:
  void exiting();

 private slots:
  void exitTimerData();

 private:
  pthread_t rtp_thread;
  QTimer *rtp_exit_timer;
  void *rtp_callback_priv;
};


#endif  // SYRTP_SERVER_H
