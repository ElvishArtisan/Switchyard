// rtp_callback.cpp
//
// Realtime RTP callback for AoIP
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <errno.h>
#include <poll.h>
#include <syslog.h>
#include <unistd.h>

#include "routing.h"
#include "rtp.h"

void RTPCallback(int sock,Routing *r,void *priv)
{
  struct sockaddr_in sa;
  struct sockaddr_in play_sa;
  struct msghdr hdr;
  struct iovec iovs[1];
  uint8_t buffer[1500];
  char mc[3000];
  struct cmsghdr *chdr=NULL;
  struct pollfd fds;
  unsigned i;
  uint32_t dst;
  ssize_t n;

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
	return;
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
	for(i=0;i<r->src_slots;i++) {
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
}
