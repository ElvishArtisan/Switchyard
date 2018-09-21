// sygpio_server.cpp
//
// Livewire GPIO Server
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
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

#include "sygpio_server.h"
#include "sysyslog.h"

SyGpioBundleEvent::SyGpioBundleEvent(SyGpioBundleEvent::Type type,
				     const QHostAddress &orig_addr,
				     uint16_t orig_port,
				     int srcnum,const QString &code)
{
  event_type=type;
  event_origin_address=orig_addr;
  event_origin_port=orig_port;
  event_source_number=srcnum;
  event_code=code;
}


SyGpioBundleEvent::Type SyGpioBundleEvent::type() const
{
  return event_type;
}


QHostAddress SyGpioBundleEvent::originAddress() const
{
  return event_origin_address;
}


uint16_t SyGpioBundleEvent::originPort() const
{
  return event_origin_port;
}


int SyGpioBundleEvent::sourceNumber() const
{
  return event_source_number;
}


QString SyGpioBundleEvent::code() const
{
  return event_code;
}


QString SyGpioBundleEvent::dump() const
{
  QString str="GPIO Bundle Event\n";
  if(type()==SyGpioBundleEvent::TypeGpi) {
    str+="Type: GPI\n";
  }
  else {
    str+="Type: GPO\n";
  }
  str+=QString().sprintf("Source Number: %d\n",sourceNumber());
  str+="Code: "+code()+"\n";
  str+=QString().sprintf("Origin: %s:%d\n",
			 (const char *)originAddress().toString().toUtf8(),
			 originPort());
  return str;
}




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


QString SyGpioEvent::dump() const
{
  QString str="GPIO Event\n";
  if(type()==SyGpioEvent::TypeGpi) {
    str+="Type: GPI\n";
  }
  else {
    str+="Type: GPO\n";
  }
  str+=QString().sprintf("Source Number: %d\n",sourceNumber());
  str+=QString().sprintf("Line: %d\n",line()+1);
  str+=QString().sprintf("State: %d\n",state());
  str+=QString().sprintf("isPulse: %d\n",isPulse());
  str+=QString().sprintf("Origin: %s:%d\n",
			 (const char *)originAddress().toString().toUtf8(),
			 originPort());
  return str;
}




SyGpioServer::SyGpioServer(SyRouting *r,QObject *parent)
  : QObject(parent)
{
  gpio_routing=r;
  gpio_eth_monitor=NULL;

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


SyGpioServer::SyGpioServer(SyRouting *r,SyEthMonitor *ethmon,QObject *parent)
  : QObject(parent)
{
  gpio_routing=r;
  gpio_eth_monitor=ethmon;
  connect(gpio_eth_monitor,SIGNAL(startedRunning()),
	  this,SLOT(interfaceStartedData()));
  connect(gpio_eth_monitor,SIGNAL(stoppedRunning()),
	  this,SLOT(interfaceStoppedData()));

  //
  // GPI Socket
  //
  gpio_gpi_socket=new SyMcastSocket(SyMcastSocket::ReadWrite,this);
  gpio_gpi_socket->bind(r->nicAddress(),SWITCHYARD_GPIO_GPI_PORT);
  if(gpio_eth_monitor->isRunning()) {
    gpio_gpi_socket->subscribe(SWITCHYARD_GPIO_ADDRESS);
  }
  connect(gpio_gpi_socket,SIGNAL(readyRead()),
	  this,SLOT(gpiReadyReadData()));

  //
  // GPO Socket
  //
  gpio_gpo_socket=new SyMcastSocket(SyMcastSocket::ReadWrite,this);
  gpio_gpo_socket->bind(r->nicAddress(),SWITCHYARD_GPIO_GPO_PORT);
  if(gpio_eth_monitor->isRunning()) {
    gpio_gpo_socket->subscribe(SWITCHYARD_GPIO_ADDRESS);
  }
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

  //
  // Send Packets
  //
  gpio_gpi_socket->writeDatagram((const char *)data,28,
				 QHostAddress(SWITCHYARD_GPIO_ADDRESS),
				 SWITCHYARD_GPIO_GPI_PORT);
  gpio_serial++;
  data[4]=0xFF&(gpio_serial>>24);
  data[5]=0xFF&(gpio_serial>>16);
  data[6]=0xFF&(gpio_serial>>8);
  data[7]=0xFF&gpio_serial;
  gpio_gpi_socket->writeDatagram((const char *)data,28,
				 QHostAddress(SWITCHYARD_GPIO_ADDRESS),
				 SWITCHYARD_GPIO_GPI_PORT);
  gpio_serial+=2;
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

  //
  // Send Packets
  //
  gpio_gpo_socket->writeDatagram((const char *)data,60,
				 QHostAddress(SWITCHYARD_GPIO_ADDRESS),
				 SWITCHYARD_GPIO_GPO_PORT);
  gpio_serial++;
  data[4]=0xFF&(gpio_serial>>24);
  data[5]=0xFF&(gpio_serial>>16);
  data[6]=0xFF&(gpio_serial>>8);
  data[7]=0xFF&gpio_serial;
  gpio_gpo_socket->writeDatagram((const char *)data,60,
				 QHostAddress(SWITCHYARD_GPIO_ADDRESS),
				 SWITCHYARD_GPIO_GPO_PORT);
  gpio_serial+=2;
}


void SyGpioServer::interfaceStartedData()
{
  gpio_gpi_socket->subscribe(SWITCHYARD_GPIO_ADDRESS);
  gpio_gpo_socket->subscribe(SWITCHYARD_GPIO_ADDRESS);
}


void SyGpioServer::interfaceStoppedData()
{
  gpio_gpi_socket->unsubscribe(SWITCHYARD_GPIO_ADDRESS);
  gpio_gpo_socket->unsubscribe(SWITCHYARD_GPIO_ADDRESS);
}


void SyGpioServer::gpiReadyReadData()
{
  char data[1500];
  int n;
  uint32_t serial;
  QHostAddress addr;
  uint16_t port=0;
  SyGpioEvent *e=NULL;
  SyGpioBundleEvent *eb=NULL;
  int srcnum=0;
  QString code;

  while((n=gpio_gpi_socket->readDatagram(data,1500,&addr,&port))>0) {
    serial=((0xFF&data[4])<<24)+((0xFF&data[5])<<16)+((0xFF&data[6])<<8)+
      (0xFF&data[7]);
    if((gpio_src_addr_serials[addr.toIPv4Address()]!=serial)&&
       (gpio_src_addr_serials[addr.toIPv4Address()]!=(serial-1))) {
      gpio_src_addr_serials[addr.toIPv4Address()]=serial;
      uint16_t count=((0xFF&data[20])<<8)+(0xFF&data[21]);
      srcnum=((0xFF&data[22+1])<<8)+(0xFF&data[22+2]);
      code=gpio_gpi_codes.value(srcnum,"hhhhh");
      for(uint16_t i=0;i<count;i++) {
	unsigned offset=22+i*6;
	//
	// FIXME: Is there actually a 'pulse' component for a GPI event?
	//        We assume not here (even though there is a place for one
	//        in the Switchyard API).
	//
	//	srcnum=((0xFF&data[offset+1])<<8)+(0xFF&data[offset+2]);
	int line=0x0D-(0xff&data[offset+3]);
	bool state=(data[offset+5]&0x01)!=0;
	printf("i: %d  srcnum: %d  line: %d  state: %d\n",
	       i,srcnum,line,state);
	if(state) {
	  code.replace(line,1,"L");
	}
	else {
	  code.replace(line,1,"H");
	}
	e=new SyGpioEvent(SyGpioEvent::TypeGpi,addr,port,srcnum,line,state,
			  false);
	emit gpioReceived(e);
	emit gpiReceived(e->sourceNumber(),e->line(),e->state(),e->isPulse());
	gpio_routing->
	  setGpi(e->sourceNumber(),e->line(),e->state(),e->isPulse());
	delete e;
      }
      gpio_gpi_codes[srcnum]=code.toLower();
      eb=
	new SyGpioBundleEvent(SyGpioBundleEvent::TypeGpi,addr,port,srcnum,code);
      emit gpioReceived(eb);
      delete eb;
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
    /*
      QString str="";
      for(int i=0;i<n;i++) {
      str+=QString().sprintf("%02x:",0xff&data[i]);
      }
      printf("%s: %s\n",(const char *)addr.toString().toUtf8(),
      (const char *)str.toUtf8());
    */
    serial=((0xFF&data[4])<<24)+((0xFF&data[5])<<16)+((0xFF&data[6])<<8)+
      (0xFF&data[7]);
    /*
    printf("%s  received GPO %u from %s, serial: %u\n",
	   (const char *)QTime::currentTime().toString("hh:mm:ss").toUtf8(),
	   ((0xFF&data[23])<<8)+(0xFF&data[24]),
	   (const char *)addr.toString().toUtf8(),serial);
    */
    if((gpio_src_addr_serials[addr.toIPv4Address()]!=serial)&&
       (gpio_src_addr_serials[addr.toIPv4Address()]!=(serial-1))){
      gpio_src_addr_serials[addr.toIPv4Address()]=serial;
      e=new SyGpioEvent(SyGpioEvent::TypeGpo,addr,port,
			((0xFF&data[23])<<8)+(0xFF&data[24]),
			0x08-(0xff&data[25]),(data[27]&0x40)!=0,
			(data[27]&0x0A)!=0);
      emit gpioReceived(e);
      emit gpoReceived(e->sourceNumber(),e->line(),e->state(),e->isPulse());
      gpio_routing->setGpo(e->sourceNumber(),e->line(),e->state(),e->isPulse());
    }
    /*
    else {
      printf("*** IGNORED!!! ***\n");
    }
    */
  }
}
