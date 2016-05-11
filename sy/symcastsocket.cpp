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

#ifdef WIN32
#define SYMCASTSOCKET_BIND_MODE QUdpSocket::ReuseAddressHint
#else
#define SYMCASTSOCKET_BIND_MODE QUdpSocket::ShareAddress
#endif  // WIN32

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
  bool ret=true;

  if(mcast_recv_socket!=NULL) {
    if(!mcast_recv_socket->bind(port,SYMCASTSOCKET_BIND_MODE)) {
      SySyslog(LOG_ERR,QString().
	       sprintf("unable to bind port %u for reading [%s]",
		       port,strerror(errno)));
      ret=false;
    }    
  }

  if(mcast_send_socket!=NULL) {
    if(!mcast_send_socket->bind(addr,port,SYMCASTSOCKET_BIND_MODE)) {
      SySyslog(LOG_ERR,QString().
	       sprintf("unable to bind port %u for writing [%s]",
		       port,strerror(errno)));
      ret=false;
    }
  }
  mcast_iface_address=addr;
  return ret;
}


bool SyMcastSocket::bind(uint16_t port)
{
  bool ret=true;

  if(mcast_send_socket!=NULL) {
    SySyslog(LOG_ERR,
     "you must provide an interface address when binding a socket for writing");
    ret=false;
  }
  if(mcast_recv_socket!=NULL) {
    if(!mcast_recv_socket->bind(port,SYMCASTSOCKET_BIND_MODE)) {
      SySyslog(LOG_ERR,QString().
	       sprintf("unable to bind port %u for reading [%s]",
		       port,strerror(errno)));
      ret=false;
    }    
  }
  return ret;
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
  if(setsockopt(mcast_recv_socket->socketDescriptor(),IPPROTO_IP,
    IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq))<0) {
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
  if(setsockopt(mcast_recv_socket->socketDescriptor(),IPPROTO_IP,
		IP_DROP_MEMBERSHIP,&mreq,sizeof(mreq))<0) {
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


QString SyMcastSocket::socketErrorText(QAbstractSocket::SocketError err)
{
  QString ret=tr("Unknown socket error")+QString().sprintf(" [%u]",err);

  switch(err) {
  case QAbstractSocket::ConnectionRefusedError:
    ret=tr("connection refused");
    break;

  case QAbstractSocket::RemoteHostClosedError:
    ret=tr("remote host closed connection");
    break;

  case QAbstractSocket::HostNotFoundError:
    ret=tr("host not found");
    break;

  case QAbstractSocket::SocketAccessError:
    ret=tr("socket access error");
    break;

  case QAbstractSocket::SocketTimeoutError:
    ret=tr("operation timed out");
    break;

  case QAbstractSocket::DatagramTooLargeError:
    ret=tr("datagram too large");
    break;

  case QAbstractSocket::NetworkError:
    ret=tr("network error");
    break;

  case QAbstractSocket::AddressInUseError:
    ret=tr("address in use");
    break;

  case QAbstractSocket::SocketAddressNotAvailableError:
    ret=tr("address not available");
    break;

  case QAbstractSocket::UnsupportedSocketOperationError:
    ret=tr("unsupported socket operation");
    break;

  default:
    break;
  }

  return ret;
}


void SyMcastSocket::readyReadData()
{
  emit readyRead();
}
