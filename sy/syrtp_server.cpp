// syrtp_server.cpp
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

#include "syconfig.h"
#include "syrtp_server.h"

struct rtp_cb_data
{
  SyRouting *routing;
  void *priv;
} cb_data;

void *ThreadCallback(void *p)
{
  struct rtp_cb_data *cb_data=(struct rtp_cb_data *)p;
  struct sockaddr_in sa;
  int read_sock;
  int write_sock;
  long sockopt;
  QHostAddress addr;

  //
  // Open the read socket
  //
  read_sock=cb_data->routing->subscriptionSocket();
  sockopt=1;
  setsockopt(read_sock,SOL_SOCKET,SO_REUSEADDR,&sockopt,sizeof(sockopt));
  setsockopt(read_sock,IPPROTO_IP,IP_PKTINFO,&sockopt,sizeof(sockopt));
  memset(&sa,0,sizeof(sa));
  sa.sin_family=AF_INET;
  sa.sin_port=htons(SWITCHYARD_RTP_PORT);
  sa.sin_addr.s_addr=htonl(INADDR_ANY);
  if(bind(read_sock,(struct sockaddr *)&sa,sizeof(sa))<0) {
    syslog(LOG_ERR,"unable to bind RTP socket [%s]",strerror(errno));
    global_exiting=true;
    return NULL;
  }

  //
  // Open the write socket
  //
  if((write_sock=socket(PF_INET,SOCK_DGRAM,IPPROTO_IP))<0) {
    syslog(LOG_ERR,"unable to create RTP socket [%s]",strerror(errno));
    exit(256);
  }
  sockopt=1;
  setsockopt(write_sock,SOL_SOCKET,SO_REUSEADDR,&sockopt,sizeof(sockopt));
  setsockopt(write_sock,IPPROTO_IP,IP_PKTINFO,&sockopt,sizeof(sockopt));
  memset(&sa,0,sizeof(sa));
  sa.sin_family=AF_INET;
  sa.sin_port=htons(SWITCHYARD_RTP_PORT);
  sa.sin_addr.s_addr=cb_data->routing->nic_addr;
  if(bind(write_sock,(struct sockaddr *)&sa,sizeof(sa))<0) {
    syslog(LOG_ERR,"unable to bind RTP socket [%s]",strerror(errno));
    global_exiting=true;
    return NULL;
  }

  RtpCallback(read_sock,write_sock,cb_data->routing,cb_data->priv);
  close(read_sock);
  return NULL;
}


SyRtpServer::SyRtpServer(SyRouting *routing,void *priv,QObject *parent)
  : QObject(parent)
{
  pthread_attr_t pthread_attr;

  //
  // Initilialize Subscriptions
  //
  for(unsigned i=0;i<routing->srcSlots();i++) {
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


void SyRtpServer::exitTimerData()
{
  if(global_exiting) {
    rtp_exit_timer->stop();
    pthread_join(rtp_thread,NULL);
    emit exiting();
  }
}
