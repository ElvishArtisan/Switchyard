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
#include <poll.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>

#include "config.h"
#include "rtp.h"

void *RTPCallback(void *p)
{
  Routing *r=(Routing *)p;
  int sock;
  struct sockaddr_in sa;
  struct sockaddr_in play_sa;
  long sockopt;
  QHostAddress addr;

  ssize_t n;
  uint8_t buffer[1500];
  struct iovec iovs[1];
  char mc[3000];
  struct msghdr hdr;
  struct cmsghdr *chdr=NULL;
  struct pollfd fds;
  unsigned i;
  uint32_t dst;

  //
  // Initialize Transmit Headers
  //
  memset(&play_sa,0,sizeof(play_sa));
  play_sa.sin_family=AF_INET;
  play_sa.sin_port=htons(SWITCHYARD_RTP_PORT);

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

  //
  // Initialize Stream Router
  //
  memset(&sa,0,sizeof(sa));
  memset(&hdr,0,sizeof(hdr));
  hdr.msg_name=&sa;
  hdr.msg_namelen=sizeof(sa);
  hdr.msg_iov=iovs;
  hdr.msg_iovlen=1;
  iovs[0].iov_base=buffer;
  iovs[0].iov_len=sizeof(buffer);
  hdr.msg_control=mc;
  hdr.msg_controllen=sizeof(mc);
  hdr.msg_flags=0;
  memset(&fds,0,sizeof(fds));
  fds.fd=sock;
  fds.events=POLLIN;

  //
  // Process RTP Data
  //
  while(!global_exiting) {
    switch(poll(&fds,1,1)) {
    case -1:
      syslog(LOG_WARNING,"poll() returned error [%s]",strerror(errno));
      break;

    case 0:
      if(global_exiting) {
	close(sock);
	return NULL;
      }
      break;

    default:
      n=recvmsg(sock,&hdr,0);
      dst=0;
      chdr=CMSG_FIRSTHDR(&hdr);
      while(chdr!=NULL) {
	if(chdr->cmsg_type==IP_PKTINFO) {
	  dst=((struct in_pktinfo *)CMSG_DATA(chdr))->ipi_addr.s_addr;
	}
	chdr=CMSG_NXTHDR(&hdr,chdr);
      }
      if(dst>0) {
	//printf("received %ld bytes from %s.\n",n,
	//       (const char *)Routing::dumpAddress(dst).toAscii());
	for(i=0;i<SWITCHYARD_SLOTS;i++) {
	  if(r->src_enabled[i]&&(r->src_addr[i]!=0)&&(dst==r->dst_addr[i])) {
	    play_sa.sin_addr.s_addr=r->src_addr[i];
	    sendto(sock,buffer,n,0,(struct sockaddr *)(&play_sa),
		   sizeof(play_sa));
	  }
	}
	break;
      }
    }
  }
  close(sock);
  return NULL;
}


RTPServer::RTPServer(Routing *routing,QObject *parent)
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
  pthread_attr_init(&pthread_attr);
  pthread_attr_setschedpolicy(&pthread_attr,SCHED_FIFO);
  pthread_create(&rtp_thread,&pthread_attr,RTPCallback,routing);
}


void RTPServer::exitTimerData()
{
  if(global_exiting) {
    rtp_exit_timer->stop();
    pthread_join(rtp_thread,NULL);
    emit exiting();
  }
}
