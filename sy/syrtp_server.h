// syrtp_server.h
//
// Real Time Protocol
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
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
