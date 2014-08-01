// gpio_server.cpp
//
// Livewire GPIO Server
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#include "gpio_server.h"

GpioServer::GpioServer(Routing *r,QObject *parent)
  : QObject(parent)
{
  gpio_routing=r;

  //
  // GPI Sockets
  //
  gpio_gpi_recv_socket=CreateRecvSocket(SWITCHYARD_GPIO_GPI_PORT);
  connect(gpio_gpi_recv_socket,SIGNAL(readyRead()),
	  this,SLOT(gpiReadyReadData()));
  gpio_gpi_send_socket=CreateSendSocket(SWITCHYARD_GPIO_GPI_PORT);

  //
  // GPO Sockets
  //
  gpio_gpo_recv_socket=CreateRecvSocket(SWITCHYARD_GPIO_GPO_PORT);
  connect(gpio_gpo_recv_socket,SIGNAL(readyRead()),
	  this,SLOT(gpoReadyReadData()));
  gpio_gpo_send_socket=CreateSendSocket(SWITCHYARD_GPIO_GPO_PORT);
}


GpioServer::~GpioServer()
{
  delete gpio_gpi_send_socket;
  delete gpio_gpi_recv_socket;
  delete gpio_gpo_send_socket;
  delete gpio_gpo_recv_socket;
}


void GpioServer::sendGpi(int gpi,int line,bool state,bool pulse)
{
  syslog(LOG_DEBUG,"sending gpi: src: %d  line: %d  state: %d  pulse: %d",
	 gpi,line,state,pulse);

  char data[28]={0x03,0x00,0x02,0x07,0xC6,0x04,0x55,0x1E,
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 'I','N','D','I',0x00,0x01,0x00,0x00,
		 0x00,0x00,0x07,0x00};

  //
  // Serial Number
  //
  data[4]=0xFF&(gpio_serial>>24);
  data[5]=0xFF&(gpio_serial>>16);
  data[6]=0xFF&(gpio_serial>>8);
  data[7]=0xFF&gpio_serial;
  gpio_serial++;

  //
  // Livewire Number
  //
  data[23]=0xFF&(gpi>>8);
  data[24]=0xFF&gpi;

  //
  // Line Number
  //
  data[25]=0xFF&(0x0D-line);

  //
  // State
  //
  data[27]=0xFF&state;

  gpio_gpi_send_socket->
    writeDatagram(data,28,QHostAddress(SWITCHYARD_GPIO_ADDRESS),
		  SWITCHYARD_GPIO_GPI_PORT);
}


void GpioServer::sendGpo(int gpo,int line,bool state,bool pulse)
{
  syslog(LOG_DEBUG,"sending gpo: src: %d  line: %d  state: %d  pulse: %d",
	 gpo,line,state,pulse);

  char data[60]={0x03,0x00,0x02,0x07,0x36,0x0B,0x97,0xA9,
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 'W','R','N','I',0x00,0x04,0x00,0x03,0xF6,
		 0x05,0x07,0xCA,0xFF,0xFF,0xFF,0xFD,0x07,
		 0x01,0xFF,0xFF,0xFF,0xFC,0x09,0x00,0x02,
		 0x15,0x07,0x00,0x12,0x00,0x8F,0xFF,0xFF,
		 0xFF,0xFF,0x09,0x00,0x02,0x15,0x07,0x00,
		 0x00,0x00,0x8F};

  //
  // Serial Number
  //
  data[4]=0xFF&(gpio_serial>>24);
  data[5]=0xFF&(gpio_serial>>16);
  data[6]=0xFF&(gpio_serial>>8);
  data[7]=0xFF&gpio_serial;
  gpio_serial++;

  //
  // Livewire Number
  //
  data[23]=0xFF&(gpo>>8);
  data[24]=0xFF&gpo;

  //
  // Line Number
  //
  data[25]=0x08-(0x07&line);

  //
  // State
  //
  if(state) {
    data[27]|=0x40;
  }
  else {
    data[27]&=~0x40;
  }

  //
  // Pulse
  //
  if(pulse) {
    data[27]|=0x0A;
  }
  else {
    data[27]&=~0x0A;
  }

  gpio_gpo_send_socket->
    writeDatagram(data,60,QHostAddress(SWITCHYARD_GPIO_ADDRESS),
		  SWITCHYARD_GPIO_GPO_PORT);
}


void GpioServer::gpiReadyReadData()
{
  char data[1500];
  int n;
  uint32_t serial;
  QHostAddress addr;

  while((n=gpio_gpi_recv_socket->readDatagram(data,1500,&addr))>0) {
    //printf("received %d bytes from %s\n",n,
    //   (const char *)addr.toString().toAscii());
    serial=((0xFF&data[4])<<24)+((0xFF&data[5])<<16)+((0xFF&data[6])<<8)+
      (0xFF&data[7]);
    if(gpio_src_addr_serials[addr.toIPv4Address()]!=(serial-1)) {
      gpio_src_addr_serials[addr.toIPv4Address()]=serial;
      //
      // FIXME: Is there actually a 'pulse' component for a GPI event?
      //        We assume not here (even though there is a place for one
      //        in the Switchyard API).
      //
      emit gpiReceived(((0xFF&data[23])<<8)+(0xFF&data[24]),
		       0x0D-(0xff&data[25]),(data[27]&0x01)!=0,false);
      gpio_routing->setGpi(((0xFF&data[23])<<8)+(0xFF&data[24]),
			   0x0D-(0xff&data[25]),(data[27]&0x01)!=0,false);
      syslog(LOG_DEBUG,"received gpi: src: %d  line: %d  state: %d  pulse: %d",
	     ((0xFF&data[23])<<8)+(0xFF&data[24]),
	     0x0D-(0xff&data[25]),(data[27]&0x01)!=0,false);
    }
    
  }
}


void GpioServer::gpoReadyReadData()
{
  char data[1500];
  int n;
  uint32_t serial;
  QHostAddress addr;

  while((n=gpio_gpo_recv_socket->readDatagram(data,1500,&addr))>0) {
    //printf("received %d bytes from %s\n",n,
    //   (const char *)addr.toString().toAscii());
    serial=((0xFF&data[4])<<24)+((0xFF&data[5])<<16)+((0xFF&data[6])<<8)+
      (0xFF&data[7]);
    if(gpio_src_addr_serials[addr.toIPv4Address()]!=(serial-1)) {
      gpio_src_addr_serials[addr.toIPv4Address()]=serial;
      emit gpoReceived(((0xFF&data[23])<<8)+(0xFF&data[24]),
		       0x08-(0xff&data[25]),(data[27]&0x40)!=0,
		       (data[27]&0x0A)!=0);
      gpio_routing->setGpo(((0xFF&data[23])<<8)+(0xFF&data[24]),
			   0x08-(0xff&data[25]),(data[27]&0x40)!=0,
			   (data[27]&0x0A)!=0);
      syslog(LOG_DEBUG,"received gpo: src: %d  line: %d  state: %d  pulse: %d",
	     ((0xFF&data[23])<<8)+(0xFF&data[24]),
	     0x08-(0xff&data[25]),(data[27]&0x40)!=0,(data[27]&0x0A)!=0);
    }
  }
}


QUdpSocket *GpioServer::CreateSendSocket(uint16_t port)
{
  QUdpSocket *socket=new QUdpSocket(this);
  socket->bind(gpio_routing->nicAddress(),port);
  return socket;
}


QUdpSocket *GpioServer::CreateRecvSocket(uint16_t port)
{
  int sock=-1;
  long sockopt;
  struct sockaddr_in sa;
  struct ip_mreqn mreq;
  QHostAddress addr;

  //
  // GPO Receive Socket
  //
  if((sock=socket(PF_INET,SOCK_DGRAM,IPPROTO_IP))<0) {
    syslog(LOG_ERR,"unable to create gpo socket [%s]",strerror(errno));
    exit(256);
  }
  sockopt=fcntl(sock,F_GETFL,NULL);
  sockopt|=O_NONBLOCK;
  fcntl(sock,F_SETFL,sockopt);
  sockopt=1;
  setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&sockopt,sizeof(sockopt));
  memset(&mreq,0,sizeof(mreq));
  addr.setAddress(SWITCHYARD_GPIO_ADDRESS);
  mreq.imr_multiaddr.s_addr=htonl(addr.toIPv4Address());
  mreq.imr_address.s_addr=htonl(gpio_routing->nicAddress().toIPv4Address());
  mreq.imr_ifindex=0;
#ifdef OSX
  if(setsockopt(sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq))<0) {
#else
  if(setsockopt(sock,SOL_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq))<0) {
#endif  // OSX
    syslog(LOG_ERR,"unable to subscribe to gpo group [%s]",strerror(errno));
    exit(256);
  }
  memset(&sa,0,sizeof(sa));
  sa.sin_family=AF_INET;
  sa.sin_port=htons(port);
  sa.sin_addr.s_addr=htonl(INADDR_ANY);
  if(bind(sock,(struct sockaddr *)&sa,sizeof(sa))<0) {
    syslog(LOG_ERR,"unable to bind gpo port [%s]",strerror(errno));
    exit(256);
  }
  QUdpSocket *socket=new QUdpSocket(this);
  socket->setSocketDescriptor(sock,QAbstractSocket::BoundState);
  return socket;
}
