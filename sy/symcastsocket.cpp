// symcastsocket.cpp
//
// A QUdpSocket for multicasting.
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef WIN32
#include <Winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#else
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#endif  // WIN32

#include "sysyslog.h"
#include "symcastsocket.h"

SyMcastSocket::SyMcastSocket(Mode mode,QObject *parent)
  : QObject(parent)
{
  mcast_mode=mode;
  mcast_send_socket=NULL;
  mcast_recv_socket=NULL;

  if((mode==SyMcastSocket::WriteOnly)||(mode==SyMcastSocket::ReadWrite)) {
    mcast_send_socket=new QUdpSocket(this);
  }
  if((mode==SyMcastSocket::ReadOnly)||(mode==SyMcastSocket::ReadWrite)) {
    mcast_recv_socket=new QUdpSocket(this);
    connect(mcast_recv_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  }
}

SyMcastSocket::~SyMcastSocket()
{
  if(mcast_send_socket!=NULL) {
    delete mcast_send_socket;
  }
  if(mcast_recv_socket!=NULL) {
    delete mcast_recv_socket;
  }
}


SyMcastSocket::Mode SyMcastSocket::mode() const
{
  return mcast_mode;
}


bool SyMcastSocket::bind(const QHostAddress &addr,uint16_t port)
{
  if(mcast_recv_socket!=NULL) {
    if(!mcast_recv_socket->bind(addr,port,QUdpSocket::ShareAddress)) {
      SySyslog(LOG_ERR,QString().
	       sprintf("unable to bind port %u for reading [%s]",
		       port,strerror(errno)));
      exit(256);
    }    
  }

  if(mcast_send_socket!=NULL) {
    if(!mcast_send_socket->bind(addr,port,QUdpSocket::ShareAddress)) {
      SySyslog(LOG_ERR,QString().
	       sprintf("unable to bind port %u for writing [%s]",
		       port,strerror(errno)));
      exit(256);
    }
  }
  mcast_iface_address=addr;
  return true;
}


bool SyMcastSocket::bind(uint16_t port)
{
  if(mcast_send_socket!=NULL) {
    SySyslog(LOG_ERR,
     "you must provide an interface address when binding a socket for writing");
    exit(256);
  }
  if(mcast_recv_socket!=NULL) {
    if(!mcast_recv_socket->bind(port,QUdpSocket::ShareAddress)) {
      SySyslog(LOG_ERR,QString().
	       sprintf("unable to bind port %u for reading [%s]",
		       port,strerror(errno)));
      exit(256);
    }    
  }
  return true;
}


qint64 SyMcastSocket::readDatagram(char *data,qint64 len,
				   QHostAddress *addr,quint16 *port)
{
  if(mcast_recv_socket==NULL) {
    SySyslog(LOG_ERR,"attempted to read from a write-only socket");
    exit(256);
  }
  return mcast_recv_socket->readDatagram(data,len,addr,port);
}


qint64 SyMcastSocket::writeDatagram(const char *data,qint64 len,
				    const QHostAddress &addr,quint16 port)
{
  if(mcast_send_socket==NULL) {
    SySyslog(LOG_ERR,"attempted to write to a read-only socket");
    exit(256);
  }
  return mcast_send_socket->writeDatagram(data,len,addr,port);
}


qint64 SyMcastSocket::writeDatagram(const QByteArray &datagram,
				    const QHostAddress &addr,quint16 port)
{
  if(mcast_send_socket==NULL) {
    SySyslog(LOG_ERR,"attempted to write to a read-only socket");
    exit(256);
  }
  return mcast_send_socket->writeDatagram(datagram,addr,port);
}


void SyMcastSocket::subscribe(const QHostAddress &addr)
{
  if(mcast_recv_socket==NULL) {
    SySyslog(LOG_ERR,"cannot subscribe on a write-only socket");
    exit(256);
  }

#ifdef WIN32
  struct ip_mreq_source imr;

  memset(&imr,0,sizeof(imr));
  imr.imr_multiaddr.s_addr=htonl(addr.toIPv4Address());
  imr.imr_interface.s_addr=htonl(mcast_iface_address.toIPv4Address());
  if(setsockopt(mcast_recv_socket->socketDescriptor(),IPPROTO_IP,
		IP_ADD_MEMBERSHIP,(char *)&imr,sizeof(imr))<0) {
    SySyslog(LOG_ERR,QString().
	     sprintf("unable to subscribe to group %s [%s]",
		     (const char *)addr.toString().toAscii(),strerror(errno)));
    exit(256);
  }
#else
  struct ip_mreqn mreq;

  memset(&mreq,0,sizeof(mreq));
  mreq.imr_multiaddr.s_addr=htonl(addr.toIPv4Address());
  mreq.imr_address.s_addr=htonl(mcast_iface_address.toIPv4Address());
  mreq.imr_ifindex=0;
#ifdef OSX
  if(setsockopt(mcast_recv_socket->socketDescriptor(),IPPROTO_IP,
    IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq))<0) {
#else
  if(setsockopt(mcast_recv_socket->socketDescriptor(),SOL_IP,
    IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq))<0) {
#endif  // OSX
    SySyslog(LOG_ERR,QString().
      sprintf("unable to subscribe to group %s [%s]",
        (const char *)addr.toString().toAscii(),strerror(errno)));
    exit(256);
  }
#endif // WIN32
}


void SyMcastSocket::subscribe(const QString &addr)
{
  subscribe(QHostAddress(addr));
}


void SyMcastSocket::unsubscribe(const QHostAddress &addr)
{
  if(mcast_recv_socket==NULL) {
    SySyslog(LOG_ERR,"cannot unsubscribe on a write-only socket");
    exit(256);
  }

#ifdef WIN32
  struct ip_mreq_source imr;

  memset(&imr,0,sizeof(imr));
  imr.imr_multiaddr.s_addr=htonl(addr.toIPv4Address());
  imr.imr_interface.s_addr=htonl(mcast_iface_address.toIPv4Address());
  if(setsockopt(mcast_recv_socket->socketDescriptor(),IPPROTO_IP,
		IP_DROP_MEMBERSHIP,(char *)&imr,sizeof(imr))<0) {
    SySyslog(LOG_ERR,QString().
	     sprintf("unable to subscribe to group %s [%s]",
		     (const char *)addr.toString().toAscii(),strerror(errno)));
    exit(256);
  }
#else
  struct ip_mreqn mreq;

  memset(&mreq,0,sizeof(mreq));
  mreq.imr_multiaddr.s_addr=htonl(addr.toIPv4Address());
  mreq.imr_address.s_addr=htonl(mcast_iface_address.toIPv4Address());
  mreq.imr_ifindex=0;
#ifdef OSX
  if(setsockopt(mcast_recv_socket->socketDescriptor(),IPPROTO_IP,
		IP_DROP_MEMBERSHIP,&mreq,sizeof(mreq))<0) {
#else
    if(setsockopt(mcast_recv_socket->socketDescriptor(),SOL_IP,
		  IP_DROP_MEMBERSHIP,&mreq,sizeof(mreq))<0) {
#endif  // OSX
      SySyslog(LOG_ERR,QString().
	       sprintf("unable to unsubscribe from group %s [%s]",
		      (const char *)addr.toString().toAscii(),strerror(errno)));
    exit(256);
  }
#endif  // WIN32
}


void SyMcastSocket::unsubscribe(const QString &addr)
{
  unsubscribe(QHostAddress(addr));
}


void SyMcastSocket::readyReadData()
{
  emit readyRead();
}
