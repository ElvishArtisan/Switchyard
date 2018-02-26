// syrtp_server.cpp
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

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#ifndef WIN32
#include <poll.h>
#endif  // WIN32

#include "syconfig.h"
#include "syrtp_server.h"
#include "sysyslog.h"

bool __rtp_shutting_down=false;

struct rtp_cb_data
{
  SyRouting *routing;
  void *(*callback_func)(unsigned dst_slot,const char *data,int len,SyRouting *,void *);
  void *priv;
} cb_data;

void *__RtpServer_ThreadCallback(void *p)
{
#ifndef WIN32
  struct rtp_cb_data *cb_data=(struct rtp_cb_data *)p;
  struct sockaddr_in sa;
  struct sockaddr_in play_sa;
  struct msghdr hdr;
  int read_sock;
  int write_sock;
  long sockopt;
  QHostAddress addr;
  struct iovec iovs[1];
  uint8_t buffer[1500];
  char mc[3000];
  struct cmsghdr *chdr=NULL;
  struct pollfd fds;
  uint32_t dst;
  ssize_t n;

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
    SySyslog(LOG_ERR,QString().
	     sprintf("unable to bind RTP socket [%s]",strerror(errno)));
    __rtp_shutting_down=true;
    return NULL;
  }

  //
  // Open the write socket
  //
  write_sock=cb_data->routing->rtpSendSocket();
  sockopt=1;
  setsockopt(write_sock,SOL_SOCKET,SO_REUSEADDR,&sockopt,sizeof(sockopt));
  setsockopt(write_sock,IPPROTO_IP,IP_PKTINFO,&sockopt,sizeof(sockopt));
  memset(&sa,0,sizeof(sa));
  sa.sin_family=AF_INET;
  sa.sin_port=htons(SWITCHYARD_RTP_PORT);
  sa.sin_addr.s_addr=cb_data->routing->nic_addr;
  if(bind(write_sock,(struct sockaddr *)&sa,sizeof(sa))<0) {
    SySyslog(LOG_ERR,QString().
	     sprintf("unable to bind RTP socket [%s]",strerror(errno)));
    __rtp_shutting_down=true;
    return NULL;
  }

  //
  // Initialize Transmit Headers
  //
  memset(&play_sa,0,sizeof(play_sa));
  play_sa.sin_family=AF_INET;
  play_sa.sin_port=htons(SWITCHYARD_RTP_PORT);

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
  fds.fd=read_sock;
  fds.events=POLLIN;

  //
  // Process RTP Data
  //
  while(!__rtp_shutting_down) {
    switch(poll(&fds,1,100)) {
    case -1:
      SySyslog(LOG_WARNING,QString().
	       sprintf("poll() returned error [%s]",strerror(errno)));
      break;

    case 0:
      if(__rtp_shutting_down) {
	close(read_sock);
	return NULL;
      }
      break;

    default:
      n=recvmsg(read_sock,&hdr,0);
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
	//       (const char *)SyRouting::dumpAddress(dst).toAscii());
	if(cb_data->callback_func!=NULL) {
	  cb_data->callback_func(dst,(char *)buffer,n,cb_data->routing,
				 cb_data->priv);
	}
	break;
      }
    }
  }

  close(write_sock);
  close(read_sock);
#endif  // WIN32
  return NULL;
}


SyRtpServer::SyRtpServer(void *(*callback_func)(unsigned,const char *,int,SyRouting *,void *),
			 void *callback_priv,SyRouting *routing,QObject *parent)
  : QObject(parent)
{
  cb_data.callback_func=callback_func;
  cb_data.priv=callback_priv;
  cb_data.routing=routing;

  pthread_attr_t pthread_attr;

  //
  // Initilialize Subscriptions
  //
  for(unsigned i=0;i<routing->dstSlots();i++) {
    if(!routing->dstAddress(i).isNull()) {
      routing->subscribe(routing->dstAddress(i));
    }
  }

  //
  // Start the Exit Monitor
  //
  rtp_exit_timer=new QTimer(this);
  connect(rtp_exit_timer,SIGNAL(timeout()),this,SLOT(exitTimerData()));
  rtp_exit_timer->start(200);

  //
  // Start the Realtime Thread
  //
  pthread_attr_init(&pthread_attr);
  pthread_attr_setschedpolicy(&pthread_attr,SCHED_FIFO);
  pthread_create(&rtp_thread,&pthread_attr,__RtpServer_ThreadCallback,&cb_data);
}


SyRtpServer::~SyRtpServer()
{
  shutdown();
}


void SyRtpServer::shutdown()
{
  __rtp_shutting_down=true;
}


void SyRtpServer::exitTimerData()
{
  if(__rtp_shutting_down) {
    rtp_exit_timer->stop();
    pthread_join(rtp_thread,NULL);
    emit exiting();
  }
}
