// sylwrp_client.cpp
//
// LWRP client implementation
//
// (C) 2014-2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <QtCore/QStringList>

#include "syastring.h"
#include "sylwrp_client.h"

SyLwrpClient::SyLwrpClient(QObject *parent)
  : QObject(parent)
{
  lwrp_connected=false;

  lwrp_socket=new QTcpSocket(this);
  connect(lwrp_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(lwrp_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
}


SyLwrpClient::~SyLwrpClient()
{
  delete lwrp_socket;
}


QString SyLwrpClient::deviceName() const
{
  return lwrp_device_name;
}


unsigned SyLwrpClient::dstSlots() const
{
  return lwrp_destinations.size();
}


unsigned SyLwrpClient::srcSlots() const
{
  return lwrp_sources.size();
}


unsigned SyLwrpClient::gpis() const
{
  return lwrp_gpis;
}


unsigned SyLwrpClient::gpos() const
{
  return lwrp_gpos;
}


QString SyLwrpClient::hostName() const
{
  return lwrp_hostname;
}


uint16_t SyLwrpClient::port() const
{
  return lwrp_port;
}


int SyLwrpClient::srcNumber(int slot) const
{
  return SyRouting::livewireNumber(lwrp_sources[slot]->streamAddress());
}


QHostAddress SyLwrpClient::srcAddress(int slot) const
{
  return lwrp_sources[slot]->streamAddress();
}


void SyLwrpClient::setSrcAddress(int slot,const QHostAddress &addr)
{
  QString cmd=QString().sprintf("SRC %d ",slot+1)+
    "RTPA:\""+addr.toString()+"\"";
  SendCommand(cmd);
}


void SyLwrpClient::setSrcAddress(int slot,const QString &addr)
{
  setSrcAddress(slot,QHostAddress(addr));
}


QString SyLwrpClient::srcName(int slot) const
{
  return lwrp_sources[slot]->name();
}


void SyLwrpClient::setSrcName(int slot,const QString &str)
{
  QString cmd=QString().sprintf("SRC %d ",slot+1)+
    "PSNM:\""+str+"\"";
  SendCommand(cmd);
}


bool SyLwrpClient::srcEnabled(int slot) const
{
  return lwrp_sources[slot]->enabled();
}


void SyLwrpClient::setSrcEnabled(int slot,bool state)
{
  QString cmd=QString().sprintf("SRC %d ",slot+1)+
    QString().sprintf("RTPE:%d",state);
  SendCommand(cmd);
}


unsigned SyLwrpClient::srcChannels(int slot) const
{
  return lwrp_sources[slot]->channels();
}


void SyLwrpClient::setSrcChannels(int slot,unsigned chans)
{
  QString cmd=QString().sprintf("SRC %d ",slot+1)+
    QString().sprintf("NCHN:%d",chans);
  SendCommand(cmd);
}


unsigned SyLwrpClient::srcPacketSize(int slot)
{
  return lwrp_sources[slot]->packetSize();
}


void SyLwrpClient::setSrcPacketSize(int slot,unsigned size)
{
  QString cmd=QString().sprintf("SRC %d ",slot+1)+
    QString().sprintf("RTPP:%d",size);
  SendCommand(cmd);
}


bool SyLwrpClient::srcShareable(int slot) const
{
  return lwrp_sources[slot]->shareable();
}


void SyLwrpClient::setSrcShareable(int slot,bool state)
{
  QString cmd=QString().sprintf("SRC %d ",slot+1)+
    QString().sprintf("SHAB:%d",state);
  SendCommand(cmd);
}


int SyLwrpClient::srcMeterLevel(int slot,int chan) const
{
  return 0;
}


QHostAddress SyLwrpClient::dstAddress(int slot) const
{
  return lwrp_destinations[slot]->streamAddress();
}


void SyLwrpClient::setDstAddress(int slot,const QHostAddress &addr)
{
  QString cmd=QString().sprintf("DST %d ",slot+1)+
    "ADDR:\""+addr.toString()+"\"";
  SendCommand(cmd);
}


void SyLwrpClient::setDstAddress(int slot,const QString &addr)
{
  setDstAddress(slot,QHostAddress(addr));
}


QString SyLwrpClient::dstName(int slot) const
{
  return lwrp_destinations[slot]->name();
}


void SyLwrpClient::setDstName(int slot,const QString &str)
{
  QString cmd=QString().sprintf("DST %d ",slot+1)+
    "NAME:\""+str+"\"";
  SendCommand(cmd);
}


unsigned SyLwrpClient::dstChannels(int slot) const
{
  return lwrp_destinations[slot]->channels();
}


void SyLwrpClient::setDstChannels(int slot,unsigned chans)
{
  QString cmd=QString().sprintf("DST %d ",slot+1)+
    "NCHN:"+QString().sprintf("%u",chans);
  SendCommand(cmd);
}


int SyLwrpClient::dstMeterLevel(int slot,int chan) const
{
  return 0;
}


bool SyLwrpClient::gpiState(int gpi,int line) const
{
  return false;
}


bool SyLwrpClient::gpiStateBySlot(int slot,int line) const
{
  return false;
}


void SyLwrpClient::setGpi(int gpi,int line,bool state,bool pulse)
{
}


bool SyLwrpClient::gpoState(int gpo,int line) const
{
  return false;
}


bool SyLwrpClient::gpoStateBySlot(int slot,int line) const
{
  return false;
}


void SyLwrpClient::setGpo(int gpo,int line,bool state,bool pulse)
{
}


QHostAddress SyLwrpClient::nicAddress() const
{
  return lwrp_nic_address;
}


void SyLwrpClient::setNicAddress(const QHostAddress &addr)
{
  QString cmd=QString("IFC ")+addr.toString();
  SendCommand(cmd);
}


void SyLwrpClient::connectToHost(const QString &hostname,uint16_t port,
				 const QString &pwd)
{
  lwrp_hostname=hostname;
  lwrp_port=port;
  lwrp_password=pwd;
  lwrp_socket->connectToHost(hostname,port);
}


void SyLwrpClient::connectedData()
{
  QString cmd="LOGIN";

  if(!lwrp_password.isEmpty()) {
    cmd+=" "+lwrp_password;
  }
  SendCommand(cmd);
  SendCommand("VER");
}


void SyLwrpClient::readyReadData()
{
  int n;
  char data[1500];

  while((n=lwrp_socket->read(data,1500))>0) {
    for(int i=0;i<n;i++) {
      switch(0xFF&data[i]) {
      case 0x0A:
	break;

      case 0x0D:
	ProcessCommand(lwrp_buffer);
	lwrp_buffer="";
	break;

      default:
	lwrp_buffer+=data[i];
	break;
      }
    }
  }
}


void SyLwrpClient::SendCommand(const QString &cmd)
{
  lwrp_socket->write((const char *)(cmd+"\r\n").toAscii());
}


void SyLwrpClient::ProcessCommand(const QString &cmd)
{
  QStringList f0=SyAString(cmd).split(" ","\"");

  if(f0[0]=="VER") {
    ProcessVER(f0);
  }
  if(f0[0]=="SRC") {
    ProcessSRC(f0);
  }
  if(f0[0]=="DST") {
    ProcessDST(f0);
  }
  if(f0[0]=="IP") {
    ProcessIP(f0);
  }
}


void SyLwrpClient::ProcessVER(const QStringList &cmds)
{
  for(int i=1;i<cmds.size();i++) {
    QStringList f1=cmds[i].split(":");
    if(f1[0]=="DEVN") {
      lwrp_device_name=f1[1].replace("\"","");
    }
    if(f1[0]=="NSRC") {
      QStringList f2=f1[1].split("/");
      for(int i=0;i<f2[0].toInt();i++) {
	lwrp_sources.push_back(new SySource());
      }
    }
    if(f1[0]=="NDST") {
      QStringList f2=f1[1].split("/");
      for(int i=0;i<f2[0].toInt();i++) {
	lwrp_destinations.push_back(new SyDestination());
      }
    }
    if(f1[0]=="NGPI") {
      lwrp_gpis=f1[1].toUInt();
    }
    if(f1[0]=="NGPO") {
      lwrp_gpos=f1[1].toUInt();
    }
  }
  SendCommand("SRC");
  SendCommand("DST");
  if((lwrp_socket->peerAddress().toIPv4Address()>>24)==127) {
    SendCommand("IFC");
  }
  SendCommand("IP");
}


void SyLwrpClient::ProcessSRC(const QStringList &cmds)
{
  bool ok=false;

  unsigned slotnum=cmds[1].toUInt(&ok)-1;
  if(ok&&(slotnum<srcSlots())) {
    SySource *src=lwrp_sources[slotnum];
    for(int i=2;i<cmds.size();i++) {
      QStringList f1=SyAString(cmds[i]).split(":","\"");
      if(f1[0]=="PSNM") {
	src->setName(f1[1].replace("\"",""));
      }
      if(f1[0]=="RTPE") {
	src->setEnabled(f1[1]=="1");
      }
      if(f1[0]=="RTPA") {
	src->setStreamAddress(QHostAddress(f1[1].replace("\"","")));
      }
    }
    if(lwrp_connected) {
      emit sourceChanged(slotnum,src);
    }
  }
}

void SyLwrpClient::ProcessDST(const QStringList &cmds)
{
  bool ok=false;

  unsigned slotnum=cmds[1].toUInt(&ok)-1;
  if(ok&&(slotnum<dstSlots())) {
    SyDestination *dst=lwrp_destinations[slotnum];
    for(int i=2;i<cmds.size();i++) {
      QStringList f1=SyAString(cmds[i]).split(":","\"");
      if(f1[0]=="NAME") {
	dst->setName(f1[1].replace("\"",""));
      }
      if(f1[0]=="ADDR") {
	dst->setStreamAddress(QHostAddress(f1[1].replace("\"","")));
      }
    }
    if(lwrp_connected) {
      emit destinationChanged(slotnum,dst);
    }
  }
}


void SyLwrpClient::ProcessIP(const QStringList &cmds)
{
  lwrp_connected=true;
  emit connected();
}


void SyLwrpClient::ProcessIFC(const QStringList &cmds)
{
  QHostAddress addr;

  if(cmds.size()==2) {
    addr.setAddress(cmds[1]);
    if(!addr.isNull()) {
      if(lwrp_nic_address!=addr) {
	lwrp_nic_address=addr;
	emit nicAddressChanged(addr);
      }
      else {
	lwrp_nic_address=addr;
      }
    }
  }
}
