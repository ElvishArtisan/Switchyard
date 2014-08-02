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

extern void RtpCallback(int read_sock,int write_sock,SyRouting *r,void *priv);

class SyRtpServer : public QObject
{
  Q_OBJECT;
 public:
  SyRtpServer(SyRouting *routing,void *priv,QObject *parent=0);

 signals:
  void exiting();

 private slots:
  void exitTimerData();

 private:
  pthread_t rtp_thread;
  QTimer *rtp_exit_timer;
};


#endif  // SYRTP_SERVER_H
