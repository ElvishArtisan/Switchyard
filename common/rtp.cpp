// rtp.cpp
//
// Real Time Protocol
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>

#include "config.h"
#include "rtp.h"

struct rtp_cb_data
{
  Routing *routing;
  void *priv;
} cb_data;

void *ThreadCallback(void *p)
{
  struct rtp_cb_data *cb_data=(struct rtp_cb_data *)p;
  struct sockaddr_in sa;
  int sock;
  long sockopt;
  QHostAddress addr;

  //
  // Open the socket
  //
  if((sock=socket(PF_INET,SOCK_DGRAM,IPPROTO_IP))<0) {
    syslog(LOG_ERR,"unable to create RTP socket [%s]",strerror(errno));
    exit(256);
  }
  sockopt=1;
  setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&sockopt,sizeof(sockopt));
  setsockopt(sock,IPPROTO_IP,IP_PKTINFO,&sockopt,sizeof(sockopt));
  memset(&sa,0,sizeof(sa));
  sa.sin_family=AF_INET;
  sa.sin_port=htons(SWITCHYARD_RTP_PORT);
  sa.sin_addr.s_addr=htonl(INADDR_ANY);
  if(bind(sock,(struct sockaddr *)&sa,sizeof(sa))<0) {
    syslog(LOG_ERR,"unable to bind RTP socket [%s]",strerror(errno));
    global_exiting=true;
    return NULL;
  }

  RTPCallback(sock,cb_data->routing,cb_data->priv);
  close(sock);
  return NULL;
}


RTPServer::RTPServer(Routing *routing,void *priv,QObject *parent)
  : QObject(parent)
{
  pthread_attr_t pthread_attr;

  //
  // Initilialize Subscriptions
  //
  for(unsigned i=0;i<SWITCHYARD_SLOTS;i++) {
    if(!routing->dstAddress(i).isNull()) {
      routing->subscribe(routing->dstAddress(i));
    }
  }

  //
  // Start the Exit Monitor
  //
  rtp_exit_timer=new QTimer(this);
  connect(rtp_exit_timer,SIGNAL(timeout()),this,SLOT(exitTimerData()));
  rtp_exit_timer->start(1000);

  //
  // Start the Realtime Thread
  //
  cb_data.routing=routing;
  cb_data.priv=priv;
  pthread_attr_init(&pthread_attr);
  pthread_attr_setschedpolicy(&pthread_attr,SCHED_FIFO);
  pthread_create(&rtp_thread,&pthread_attr,ThreadCallback,&cb_data);
}


void RTPServer::exitTimerData()
{
  if(global_exiting) {
    rtp_exit_timer->stop();
    pthread_join(rtp_thread,NULL);
    emit exiting();
  }
}