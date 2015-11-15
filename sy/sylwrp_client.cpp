// sylwrp_client.cpp
//
// LWRP client implementation
//
// (C) 2014-2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <QStringList>

#include "syastring.h"
#include "sylwrp_client.h"

SyLwrpClient::SyLwrpClient(unsigned id,QObject *parent)
  : QObject(parent)
{
  lwrp_connected=false;
  lwrp_id=id;

  lwrp_socket=new QTcpSocket(this);
  connect(lwrp_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(lwrp_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));

  lwrp_node=new SyNode();
}


SyLwrpClient::~SyLwrpClient()
{
  delete lwrp_node;
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
  return lwrp_gpis.size();
}


unsigned SyLwrpClient::gpos() const
{
  return lwrp_gpos.size();
}


QString SyLwrpClient::hostName() const
{
  return lwrp_hostname;
}


QHostAddress SyLwrpClient::hostAddress() const
{
  return lwrp_host_address;
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


SyGpioBundle *SyLwrpClient::gpiBundle(int slot) const
{
  return lwrp_gpis[slot];
}


void SyLwrpClient::setGpiCode(int slot,const QString &code)
{
  if(lwrp_gpis[slot]->code()!=code) {
    SendCommand(QString().sprintf("GPI %d ",slot+1)+code);
  }
}


SyGpo *SyLwrpClient::gpo(int slot) const
{
  return lwrp_gpos[slot];
}


void SyLwrpClient::setGpoCode(int slot,const QString &code)
{
  if(lwrp_gpos[slot]->bundle()->code()!=code) {
    SendCommand(QString().sprintf("GPO %d ",slot+1)+code);
  }
}


void SyLwrpClient::setGpoName(int slot,const QString &str)
{
  SendCommand(QString().sprintf("CFG GPO %d",slot+1)+" NAME:\""+str+"\"");
}


void SyLwrpClient::setGpoSourceAddress(int slot,const QHostAddress &s_addr,
				       int s_slot)
{
  if(s_addr.isNull()) {
    SendCommand(QString().sprintf("CFG GPO %d",slot+1)+" SRCA: FUNC:");
  }
  else {
    if(s_slot<0) {
      SendCommand(QString().sprintf("CFG GPO %d",slot+1)+" SRCA:\""+
		  s_addr.toString()+"\" FUNC:");
    }
    else {
      SendCommand(QString().sprintf("CFG GPO %d",slot+1)+" SRCA:\""+
		  s_addr.toString()+QString().sprintf("\"/%d",s_slot+1)+
		  " FUNC:FOLLOW");
    }
  }
}


void SyLwrpClient::setGpoFollow(int slot,bool state)
{
  if(state) {
    SendCommand(QString().sprintf("CFG GPO %d FUNC:FOLLOW",slot+1));
  }
  else {
    SendCommand(QString().sprintf("CFG GPO %d FUNC:",slot+1));
  }
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


void SyLwrpClient::connectToHost(const QHostAddress &addr,uint16_t port,
				 const QString &pwd)
{
  lwrp_host_address=addr;
  lwrp_port=port;
  lwrp_password=pwd;
  lwrp_socket->connectToHost(addr.toString(),port);
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
    data[n]=0;
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
  if(f0[0]=="GPI") {
    ProcessGPI(f0);
  }
  if(f0[0]=="GPO") {
    ProcessGPO(f0);
  }
  if(f0[0]=="CFG") {
    ProcessCFG(f0);
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
      lwrp_node->setDeviceName(f1[1].replace("\"",""));
    }
    if(f1[0]=="NSRC") {
      QStringList f2=f1[1].split("/");
      for(int i=0;i<f2[0].toInt();i++) {
	lwrp_sources.push_back(new SySource());
      }
      lwrp_node->setSrcSlotQuantity(f2[0].toInt());
    }
    if(f1[0]=="NDST") {
      QStringList f2=f1[1].split("/");
      for(int i=0;i<f2[0].toInt();i++) {
	lwrp_destinations.push_back(new SyDestination());
      }
      lwrp_node->setDstSlotQuantity(f2[0].toInt());
    }
    if(f1[0]=="NGPI") {
      for(int i=0;i<f1[1].toInt();i++) {
	lwrp_gpis.push_back(new SyGpioBundle());
      }
      lwrp_node->setGpiSlotQuantity(f1[1].toUInt());
    }
    if(f1[0]=="NGPO") {
      for(int i=0;i<f1[1].toInt();i++) {
	lwrp_gpos.push_back(new SyGpo());
      }
      lwrp_node->setGpoSlotQuantity(f1[1].toUInt());
    }
    if(f1[0]=="LWRP") {
      lwrp_node->setLwrpVersion(f1[1].replace("\"",""));
    }
    if(f1[0]=="PRODUCT") {
      lwrp_node->setProduct(f1[1].replace("\"",""));
    }
    if(f1[0]=="MODEL") {
      lwrp_node->setModel(f1[1].replace("\"",""));
    }
    if(f1[0]=="SVER") {
      lwrp_node->setSoftwareVersion(f1[1].replace("\"",""));
    }
  }
  if(lwrp_node->srcSlotQuantity()>0) {
    SendCommand("SRC");
  }
  if(lwrp_node->dstSlotQuantity()>0) {
    SendCommand("DST");
  }
  if(lwrp_node->gpiSlotQuantity()>0) {
    SendCommand("ADD GPI");
  }
  if(lwrp_node->gpoSlotQuantity()>0) {
    SendCommand("ADD GPO");
    SendCommand("CFG GPO");
  }
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
      if(f1[0]=="NCHN") {
	src->setChannels(f1[1].toUInt());
      }
      if(f1[0]=="PSNM") {
	src->setName(f1[1].replace("\"",""));
      }
      if(f1[0]=="RTPE") {
	src->setEnabled(f1[1]=="1");
      }
      if(f1[0]=="RTPA") {
	src->setStreamAddress(QHostAddress(f1[1].replace("\"","")));
      }
      if(f1[0]=="RTPP") {
	src->setPacketSize(f1[1].toUInt());
      }
    }
    if(lwrp_connected) {
      emit sourceChanged(lwrp_id,slotnum,*lwrp_node,*src);
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
      if(f1[0]=="NCHN") {
	dst->setChannels(f1[1].toUInt());
      }
      if(f1[0]=="ADDR") {
	dst->setStreamAddress(f1[1].replace("\"",""));
      }
    }
    if(lwrp_connected) {
      emit destinationChanged(lwrp_id,slotnum,*lwrp_node,*dst);
    }
  }
}


void SyLwrpClient::ProcessGPI(const QStringList &cmds)
{
  bool ok=false;

  if(cmds.size()==3) {
    unsigned slotnum=cmds[1].toUInt(&ok)-1;
    if(ok&&(slotnum<gpis())) {
      if(lwrp_gpis[slotnum]->code()!=cmds[2]) {
	lwrp_gpis[slotnum]->setCode(cmds[2]);
	if(lwrp_connected) {
	  emit gpiChanged(lwrp_id,slotnum,*lwrp_node,*(lwrp_gpis[slotnum]));
	}
      }
    }
  }
}


void SyLwrpClient::ProcessGPO(const QStringList &cmds)
{
  bool ok=false;

  if(cmds.size()==3) {
    unsigned slotnum=cmds[1].toUInt(&ok)-1;
    if(ok&&(slotnum<gpos())) {
      if(lwrp_gpos[slotnum]->bundle()->code()!=cmds[2]) {
	lwrp_gpos[slotnum]->bundle()->setCode(cmds[2]);
	if(lwrp_connected) {
	  emit gpoChanged(lwrp_id,slotnum,*lwrp_node,*(lwrp_gpos[slotnum]));
	}
      }
    }
  }
}


void SyLwrpClient::ProcessCFG(const QStringList &cmds)
{
  if(cmds.size()>=4) {
    bool ok=false;
    unsigned slotnum=cmds[2].toUInt(&ok)-1;
    SyGpo *gpo=lwrp_gpos[slotnum];
    for(int i=3;i<cmds.size();i++) {
      QStringList f0=SyAString(cmds[i]).split(":","\"");
      if(f0.size()==2) {
	if(f0[0]=="NAME") {
	  gpo->setName(f0[1]);
	}
	if(f0[0]=="SRCA") {
	  QStringList f1=f0[1].split(" ");
	  unsigned srcnum=f1[0].replace("\"","").toUInt(&ok);
	  if(ok) {  // Source number
	    // FIXME: This breaks with surround sound!
	    gpo->setSourceAddress(SyRouting::streamAddress(SyRouting::Stereo,
							   srcnum),-1);
	  }
	  else {  // IP address
	    QStringList f2=f1[0].replace("\"","").split("/");
	    int s_slot=-1;
	    if(f2.size()==2) {
	      s_slot=f2[1].toInt()-1;
	    }
	    gpo->setSourceAddress(QHostAddress(f2[0]),s_slot);
	  }
	}
	if(f0[0]=="FUNC") {
	  gpo->setFollow(f0[1].toLower()=="follow");
	}
      }
    }
    if(lwrp_connected) {
      emit gpoChanged(lwrp_id,slotnum,*lwrp_node,*gpo);
    }
  }
}


void SyLwrpClient::ProcessIP(const QStringList &cmds)
{
  if(cmds.size()==9) {
    lwrp_hostname=cmds[8];
    lwrp_node->setHostName(cmds[8]);
    lwrp_node->setHostAddress(QHostAddress(cmds[2]));
  }
  lwrp_connected=true;
  emit connected(lwrp_id);
}


void SyLwrpClient::ProcessIFC(const QStringList &cmds)
{
  QHostAddress addr;

  if(cmds.size()==2) {
    addr.setAddress(cmds[1]);
    if(!addr.isNull()) {
      if(lwrp_nic_address!=addr) {
	lwrp_nic_address=addr;
	emit nicAddressChanged(lwrp_id,addr);
      }
      else {
	lwrp_nic_address=addr;
      }
    }
  }
}
