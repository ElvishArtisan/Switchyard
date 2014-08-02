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

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include <sy/syrouting.h>

class SyRtpServer : public QObject
{
  Q_OBJECT;
 public:
  SyRtpServer(void *(*callback_func)(unsigned,const char *,int,SyRouting *,void *),
	      void *callback_priv,SyRouting *routing,QObject *parent=0);

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
