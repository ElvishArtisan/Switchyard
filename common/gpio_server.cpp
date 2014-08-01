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
  int sock=-1;
  long sockopt;
  struct sockaddr_in sa;
  struct ip_mreqn mreq;
  QHostAddress addr;

  //
  // Receive Socket
  //
  if((sock=socket(PF_INET,SOCK_DGRAM,IPPROTO_IP))<0) {
    syslog(LOG_ERR,"unable to create gpio socket [%s]",strerror(errno));
    exit(256);
  }
  sockopt=fcntl(sock,F_GETFL,NULL);
  sockopt|=O_NONBLOCK;
  fcntl(sock,F_SETFL,sockopt);
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
    syslog(LOG_ERR,"unable to subscribe to gpio group [%s]",strerror(errno));
    exit(256);
  }
  memset(&sa,0,sizeof(sa));
  sa.sin_family=AF_INET;
  sa.sin_port=htons(SWITCHYARD_GPIO_RECV_PORT);
  sa.sin_addr.s_addr=htonl(INADDR_ANY);
  if(bind(sock,(struct sockaddr *)&sa,sizeof(sa))<0) {
    syslog(LOG_ERR,"unable to bind gpio port [%s]",strerror(errno));
    exit(256);
  }
  gpio_recv_socket=new QUdpSocket(this);
  gpio_recv_socket->setSocketDescriptor(sock,QAbstractSocket::BoundState);
  connect(gpio_recv_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));

  //
  // Send Socket
  //
  gpio_send_socket=new QUdpSocket(this);
  gpio_send_socket->bind(gpio_routing->nicAddress(),SWITCHYARD_GPIO_SEND_PORT);
}


GpioServer::~GpioServer()
{
  delete gpio_send_socket;
  delete gpio_recv_socket;
}

void GpioServer::sendGpo(int gpi,int line,bool state,bool pulse)
{
  syslog(LOG_DEBUG,"sending gpo: gpi: %d  line: %d  state: %d  pulse: %d\n",
	 gpi,line,state,pulse);

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
  data[23]=0xFF&(gpi>>8);
  data[24]=0xFF&gpi;

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

  gpio_send_socket->writeDatagram(data,60,QHostAddress(SWITCHYARD_GPIO_ADDRESS),
				  SWITCHYARD_GPIO_RECV_PORT);
}


void GpioServer::readyReadData()
{
  char data[1500];
  int n;
  uint32_t serial;
  QHostAddress addr;

  while((n=gpio_recv_socket->readDatagram(data,1500,&addr))>0) {
    serial=((0xFF&data[4])<<24)+((0xFF&data[5])<<16)+((0xFF&data[6])<<8)+
      (0xFF&data[7]);
    if(gpio_src_addr_serials[addr.toIPv4Address()]!=(serial-1)) {
      gpio_src_addr_serials[addr.toIPv4Address()]=serial;
      emit gpoReceived(((0xFF&data[23])<<8)+(0xFF&data[24]),
		       0x08-(0xff&data[25]),(data[27]&0x40)!=0,
		       (data[27]&0x0A)!=0);
      gpio_routing->setGpi(((0xFF&data[23])<<8)+(0xFF&data[24]),
			   0x08-(0xff&data[25]),(data[27]&0x40)!=0,
			   (data[27]&0x0A)!=0);
    }
  }
}
