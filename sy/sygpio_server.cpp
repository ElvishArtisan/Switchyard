// sygpio_server.cpp
//
// Livewire GPIO Server
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#include "sygpio_server.h"
#include "sysyslog.h"

SyGpioEvent::SyGpioEvent(SyGpioEvent::Type type,const QHostAddress &orig_addr,
			 uint16_t orig_port,int srcnum,int line,bool state,
			 bool pulse)
{
  event_type=type;
  event_origin_address=orig_addr;
  event_origin_port=orig_port;
  event_source_number=srcnum;
  event_line=line;
  event_state=state;
  event_pulse=pulse;
}


SyGpioEvent::Type SyGpioEvent::type() const
{
  return event_type;
}


QHostAddress SyGpioEvent::originAddress() const
{
  return event_origin_address;
}


uint16_t SyGpioEvent::originPort() const
{
  return event_origin_port;
}


int SyGpioEvent::sourceNumber() const
{
  return event_source_number;
}


int SyGpioEvent::line() const
{
  return event_line;
}


bool SyGpioEvent::state() const
{
  return event_state;
}


bool SyGpioEvent::isPulse() const
{
  return event_pulse;
}




SyGpioServer::SyGpioServer(SyRouting *r,QObject *parent)
  : QObject(parent)
{
  gpio_routing=r;

  //
  // GPI Socket
  //
  gpio_gpi_socket=new SyMcastSocket(SyMcastSocket::ReadWrite,this);
  gpio_gpi_socket->bind(r->nicAddress(),SWITCHYARD_GPIO_GPI_PORT);
  gpio_gpi_socket->subscribe(SWITCHYARD_GPIO_ADDRESS);
  connect(gpio_gpi_socket,SIGNAL(readyRead()),
	  this,SLOT(gpiReadyReadData()));

  //
  // GPO Socket
  //
  gpio_gpo_socket=new SyMcastSocket(SyMcastSocket::ReadWrite,this);
  gpio_gpo_socket->bind(r->nicAddress(),SWITCHYARD_GPIO_GPO_PORT);
  gpio_gpo_socket->subscribe(SWITCHYARD_GPIO_ADDRESS);
  connect(gpio_gpo_socket,SIGNAL(readyRead()),
	  this,SLOT(gpoReadyReadData()));
}


SyGpioServer::~SyGpioServer()
{
  delete gpio_gpo_socket;
  delete gpio_gpi_socket;
}


void SyGpioServer::sendGpi(int gpi,int line,bool state,bool pulse)
{
  uint8_t data[28]={0x03,0x00,0x02,0x07,0xC6,0x04,0x55,0x1E,
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

  gpio_gpi_socket->writeDatagram((const char *)data,28,
				 QHostAddress(SWITCHYARD_GPIO_ADDRESS),
				 SWITCHYARD_GPIO_GPI_PORT);
}


void SyGpioServer::sendGpo(int gpo,int line,bool state,bool pulse)
{
  uint8_t data[60]={0x03,0x00,0x02,0x07,0x36,0x0B,0x97,0xA9,
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

  gpio_gpo_socket->writeDatagram((const char *)data,60,
				 QHostAddress(SWITCHYARD_GPIO_ADDRESS),
				 SWITCHYARD_GPIO_GPO_PORT);
}


void SyGpioServer::gpiReadyReadData()
{
  char data[1500];
  int n;
  uint32_t serial;
  QHostAddress addr;
  uint16_t port=0;
  SyGpioEvent *e=NULL;

  while((n=gpio_gpi_socket->readDatagram(data,1500,&addr,&port))>0) {
    serial=((0xFF&data[4])<<24)+((0xFF&data[5])<<16)+((0xFF&data[6])<<8)+
      (0xFF&data[7]);
    if(gpio_src_addr_serials[addr.toIPv4Address()]!=(serial-1)) {
      gpio_src_addr_serials[addr.toIPv4Address()]=serial;
      //
      // FIXME: Is there actually a 'pulse' component for a GPI event?
      //        We assume not here (even though there is a place for one
      //        in the Switchyard API).
      //
      e=new SyGpioEvent(SyGpioEvent::TypeGpi,addr,port,
			((0xFF&data[23])<<8)+(0xFF&data[24]),
		       0x0D-(0xff&data[25]),(data[27]&0x01)!=0,false);
      emit gpioReceived(e);
      emit gpiReceived(e->sourceNumber(),e->line(),e->state(),e->isPulse());
      gpio_routing->setGpi(e->sourceNumber(),e->line(),e->state(),e->isPulse());
      delete e;
    }
    
  }
}


void SyGpioServer::gpoReadyReadData()
{
  char data[1500];
  int n;
  uint32_t serial;
  QHostAddress addr;
  uint16_t port=0;
  SyGpioEvent *e=NULL;

  while((n=gpio_gpo_socket->readDatagram(data,1500,&addr,&port))>0) {
    serial=((0xFF&data[4])<<24)+((0xFF&data[5])<<16)+((0xFF&data[6])<<8)+
      (0xFF&data[7]);
    if(gpio_src_addr_serials[addr.toIPv4Address()]!=(serial-1)) {
      gpio_src_addr_serials[addr.toIPv4Address()]=serial;
      e=new SyGpioEvent(SyGpioEvent::TypeGpo,addr,port,
			((0xFF&data[23])<<8)+(0xFF&data[24]),
			0x08-(0xff&data[25]),(data[27]&0x40)!=0,
			(data[27]&0x0A)!=0);
      emit gpioReceived(e);
      emit gpoReceived(e->sourceNumber(),e->line(),e->state(),e->isPulse());
      gpio_routing->setGpo(e->sourceNumber(),e->line(),e->state(),e->isPulse());
    }
  }
}
