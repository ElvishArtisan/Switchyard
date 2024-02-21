// sylwrp_client.cpp
//
// LWRP client implementation
//
// (C) 2014-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <QStringList>

#include "syastring.h"
#include "sylwrp_client.h"
#include "sysyslog.h"

SyLwrpClient::SyLwrpClient(unsigned id,QObject *parent)
  : QObject(parent)
{
  lwrp_connected=false;
  lwrp_watchdog_state=false;
  lwrp_id=id;
  lwrp_persistent=false;
  lwrp_timeout_interval=-1;
  lwrp_connection_error=(QAbstractSocket::SocketError)-2;

  lwrp_socket=new QTcpSocket(this);
  connect(lwrp_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(lwrp_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  connect(lwrp_socket,SIGNAL(error(QAbstractSocket::SocketError)),
	  this,SLOT(errorData(QAbstractSocket::SocketError)));

  //
  // Connection Timer
  //
  lwrp_connection_timer=new QTimer(this);
  lwrp_connection_timer->setSingleShot(true);
  connect(lwrp_connection_timer,SIGNAL(timeout()),
	  this,SLOT(connectionTimeoutData()));

  //
  // Meter Timers
  //
  lwrp_meter_timers[SyLwrpClient::InputMeter]=new QTimer(this);
  connect(lwrp_meter_timers[SyLwrpClient::InputMeter],SIGNAL(timeout()),
	  this,SLOT(inputMeterData()));
  lwrp_meter_timers[SyLwrpClient::OutputMeter]=new QTimer(this);
  connect(lwrp_meter_timers[SyLwrpClient::OutputMeter],SIGNAL(timeout()),
	  this,SLOT(outputMeterData()));

  //
  // GPIO Timers
  //
  lwrp_prev_gpi_mapper=new QSignalMapper(this);
  connect(lwrp_prev_gpi_mapper,SIGNAL(mapped(int)),
	  this,SLOT(gpiResetData(int)));
  lwrp_prev_gpo_mapper=new QSignalMapper(this);
  connect(lwrp_prev_gpo_mapper,SIGNAL(mapped(int)),
	  this,SLOT(gpoResetData(int)));

  //
  // Timeout Timer
  //
  lwrp_timeout_timer=new QTimer(this);
  lwrp_timeout_timer->setSingleShot(true);
  connect(lwrp_timeout_timer,SIGNAL(timeout()),this,SLOT(timeoutData()));

  //
  // Watchdog Timers
  //
  lwrp_watchdog_retry_timer=new QTimer(this);
  lwrp_watchdog_retry_timer->setSingleShot(true);
  connect(lwrp_watchdog_retry_timer,SIGNAL(timeout()),
	  this,SLOT(watchdogRetryData()));

  lwrp_watchdog_interval_timer=new QTimer(this);
  lwrp_watchdog_interval_timer->setSingleShot(true);
  connect(lwrp_watchdog_interval_timer,SIGNAL(timeout()),
	  this,SLOT(watchdogIntervalData()));

  // DEBUG
  connect(lwrp_socket,SIGNAL(disconnected()),this,SLOT(disconnectedData()));

  lwrp_node=new SyNode();
}


SyLwrpClient::~SyLwrpClient()
{
  delete lwrp_meter_timers[SyLwrpClient::InputMeter];
  delete lwrp_meter_timers[SyLwrpClient::OutputMeter];
  delete lwrp_node;
  delete lwrp_socket;
  delete lwrp_timeout_timer;
  delete lwrp_watchdog_interval_timer;
  delete lwrp_watchdog_retry_timer;
  delete lwrp_connection_timer;
  for(int i=0;i<lwrp_prev_gpi_timers.size();i++) {
    delete lwrp_prev_gpi_timers[i];
  }
  delete lwrp_prev_gpi_mapper;
  for(int i=0;i<lwrp_prev_gpo_timers.size();i++) {
    delete lwrp_prev_gpo_timers[i];
  }
  delete lwrp_prev_gpo_mapper;
}


unsigned SyLwrpClient::id() const
{
  return lwrp_id;
}


bool SyLwrpClient::isConnected() const
{
  return lwrp_connected;
}


SyNode *SyLwrpClient::node() const
{
  return lwrp_node;
}


QString SyLwrpClient::deviceName() const
{
  return lwrp_device_name;
}


QString SyLwrpClient::productName() const
{
  return lwrp_product_name;
}


unsigned SyLwrpClient::dstSlots() const
{
  return lwrp_destinations.size();
}


unsigned SyLwrpClient::srcSlots() const
{
  return lwrp_sources.size();
}


SySource *SyLwrpClient::src(int slot) const
{
  return lwrp_sources.at(slot);
}


SyDestination *SyLwrpClient::dst(int slot) const
{
  return lwrp_destinations.at(slot);
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
  QString cmd=QString::asprintf("SRC %d ",slot+1)+
    "RTPA:\""+addr.toString()+"\"";
  sendRawLwrp(cmd);
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
  QString cmd=QString::asprintf("SRC %d ",slot+1)+
    "PSNM:\""+str+"\"";
  sendRawLwrp(cmd);
}


QString SyLwrpClient::srcLabel(int slot) const
{
  return lwrp_sources[slot]->label();
}


void SyLwrpClient::setSrcLabel(int slot,const QString &str)
{
  QString cmd=QString::asprintf("SRC %d ",slot+1)+
    "LABL:\""+str+"\"";
  sendRawLwrp(cmd);
}


bool SyLwrpClient::srcEnabled(int slot) const
{
  return lwrp_sources[slot]->enabled();
}


void SyLwrpClient::setSrcEnabled(int slot,bool state)
{
  QString cmd=QString::asprintf("SRC %d ",slot+1)+
    QString::asprintf("RTPE:%d",state);
  sendRawLwrp(cmd);
}


unsigned SyLwrpClient::srcChannels(int slot) const
{
  return lwrp_sources[slot]->channels();
}


void SyLwrpClient::setSrcChannels(int slot,unsigned chans)
{
  QString cmd=QString::asprintf("SRC %d ",slot+1)+
    QString::asprintf("NCHN:%d",chans);
  sendRawLwrp(cmd);
}


unsigned SyLwrpClient::srcPacketSize(int slot)
{
  return lwrp_sources[slot]->packetSize();
}


void SyLwrpClient::setSrcPacketSize(int slot,unsigned size)
{
  QString cmd=QString::asprintf("SRC %d ",slot+1)+
    QString::asprintf("RTPP:%d",size);
  sendRawLwrp(cmd);
}


bool SyLwrpClient::srcShareable(int slot) const
{
  return lwrp_sources[slot]->shareable();
}


void SyLwrpClient::setSrcShareable(int slot,bool state)
{
  QString cmd=QString::asprintf("SRC %d ",slot+1)+
    QString::asprintf("SHAB:%d",state);
  sendRawLwrp(cmd);
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
  QString cmd=QString::asprintf("DST %d ",slot+1)+
    "ADDR:\""+addr.toString()+"\"";
  sendRawLwrp(cmd);
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
  QString cmd=QString::asprintf("DST %d ",slot+1)+
    "NAME:\""+str+"\"";
  sendRawLwrp(cmd);
}


unsigned SyLwrpClient::dstChannels(int slot) const
{
  return lwrp_destinations[slot]->channels();
}


void SyLwrpClient::setDstChannels(int slot,unsigned chans)
{
  QString cmd=QString::asprintf("DST %d ",slot+1)+
    "NCHN:"+QString::asprintf("%u",chans);
  sendRawLwrp(cmd);
}


int SyLwrpClient::dstMeterLevel(int slot,int chan) const
{
  return 0;
}


SyGpioBundle *SyLwrpClient::gpiBundle(int slot) const
{
  return lwrp_gpis[slot];
}


void SyLwrpClient::setGpiCode(int slot,const QString &code,int duration)
{
  if(lwrp_gpis[slot]->code()!=code) {
    if(duration>0) {
      if(lwrp_prev_gpi_timers[slot]->isActive()) {
	lwrp_prev_gpi_timers[slot]->stop();
      }
      else {
	lwrp_prev_gpi_states[slot]=lwrp_gpis[slot]->code();
      }
      lwrp_prev_gpi_timers[slot]->start(duration);
    }
    sendRawLwrp(QString::asprintf("GPI %d ",slot+1)+code);
  }
}


SyGpo *SyLwrpClient::gpo(int slot) const
{
  return lwrp_gpos[slot];
}


void SyLwrpClient::setGpoCode(int slot,const QString &code,int duration)
{
  if(lwrp_gpos[slot]->bundle()->code()!=code) {
    if(duration>0) {
      if(lwrp_prev_gpo_timers[slot]->isActive()) {
	lwrp_prev_gpo_timers[slot]->stop();
      }
      else {
	lwrp_prev_gpo_states[slot]=lwrp_gpos[slot]->bundle()->code();
      }
      lwrp_prev_gpo_timers[slot]->start(duration);
    }
    sendRawLwrp(QString::asprintf("GPO %d ",slot+1)+code);
  }
}


void SyLwrpClient::setGpoName(int slot,const QString &str)
{
  sendRawLwrp(QString::asprintf("CFG GPO %d",slot+1)+" NAME:\""+str+"\"");
}


void SyLwrpClient::setGpoSourceAddress(int slot,const QHostAddress &s_addr,
				       int s_slot)
{
  if(s_addr.isNull()) {
    sendRawLwrp(QString::asprintf("CFG GPO %d",slot+1)+" SRCA: FUNC:");
  }
  else {
    if(s_slot<0) {
      sendRawLwrp(QString::asprintf("CFG GPO %d",slot+1)+" SRCA:\""+
		  s_addr.toString()+"\" FUNC:");
    }
    else {
      sendRawLwrp(QString::asprintf("CFG GPO %d",slot+1)+" SRCA:\""+
		  s_addr.toString()+QString::asprintf("/%d\"",s_slot+1)+
		  " FUNC:FOLLOW");
    }
  }
}


void SyLwrpClient::setGpoFollow(int slot,bool state)
{
  if(state) {
    sendRawLwrp(QString::asprintf("CFG GPO %d FUNC:FOLLOW",slot+1));
  }
  else {
    sendRawLwrp(QString::asprintf("CFG GPO %d FUNC:",slot+1));
  }
}


bool SyLwrpClient::clipAlarmActive(int slot,MeterType type,int chan) const
{
  if(type==SyLwrpClient::InputMeter) {
    return lwrp_source_clip_alarms[chan][slot];
  }
  return lwrp_destination_clip_alarms[chan][slot];
}


bool SyLwrpClient::silenceAlarmActive(int slot,MeterType type,int chan) const
{
  if(type==SyLwrpClient::InputMeter) {
    return lwrp_source_silence_alarms[chan][slot];
  }
  return lwrp_destination_silence_alarms[chan][slot];
}


void SyLwrpClient::setClipMonitor(int slot,SyLwrpClient::MeterType type,
				  int lvl,int msec)
{
  switch(type) {
  case SyLwrpClient::InputMeter:
    lwrp_sources[slot]->setClipThreshold(lvl);
    lwrp_sources[slot]->setClipTimeout(msec);
    sendRawLwrp(QString::asprintf("LVL ICH %u CLIP.LEVEL:%d CLIP.TIME:%d LOW.LEVEL:%d LOW.TIME:%d",
    				  slot+1,lvl,msec,
				  lwrp_sources[slot]->silenceThreshold(),
				  lwrp_sources[slot]->silenceTimeout()));
    break;

  case SyLwrpClient::OutputMeter:
    lwrp_destinations[slot]->setClipThreshold(lvl);
    lwrp_destinations[slot]->setClipTimeout(msec);
    sendRawLwrp(QString::asprintf("LVL OCH %u CLIP.LEVEL:%d CLIP.TIME:%d LOW.LEVEL:%d LOW.TIME:%d",
				  slot+1,lvl,msec,
				  lwrp_destinations[slot]->silenceThreshold(),
				  lwrp_destinations[slot]->silenceTimeout()));
    break;

  case SyLwrpClient::LastTypeMeter:
    break;
  }
}


void SyLwrpClient::setSilenceMonitor(int slot,SyLwrpClient::MeterType type,
				     int lvl,int msec)
{
  switch(type) {
  case SyLwrpClient::InputMeter:
    lwrp_sources[slot]->setSilenceThreshold(lvl);
    lwrp_sources[slot]->setSilenceTimeout(msec);
    sendRawLwrp(QString::asprintf("LVL ICH %u CLIP.LEVEL:%d CLIP.TIME:%d LOW.LEVEL:%d LOW.TIME:%d",
				  slot+1,
				  lwrp_sources[slot]->clipThreshold(),
				  lwrp_sources[slot]->clipTimeout(),
				  lvl,msec));
    break;

  case SyLwrpClient::OutputMeter:
    lwrp_destinations[slot]->setSilenceThreshold(lvl);
    lwrp_destinations[slot]->setSilenceTimeout(msec);
    sendRawLwrp(QString::asprintf("LVL OCH %u CLIP.LEVEL:%d CLIP.TIME:%d LOW.LEVEL:%d LOW.TIME:%d",
				  slot+1,
				  lwrp_destinations[slot]->clipThreshold(),
				  lwrp_destinations[slot]->clipTimeout(),
				  lvl,msec));
    break;

  case SyLwrpClient::LastTypeMeter:
    break;
  }
}


void SyLwrpClient::startMeter(MeterType type)
{
  lwrp_meter_timers[type]->start(SYLWRP_CLIENT_METER_INTERVAL);
}


void SyLwrpClient::stopMeter(MeterType type)
{
  lwrp_meter_timers[type]->stop();
}


QHostAddress SyLwrpClient::nicAddress() const
{
  return lwrp_nic_address;
}


void SyLwrpClient::setNicAddress(const QHostAddress &addr)
{
  QString cmd=QString("IFC ")+addr.toString();
  sendRawLwrp(cmd);
}


void SyLwrpClient::connectToHost(const QHostAddress &addr,uint16_t port,
				 const QString &pwd,bool persistent)
{
  lwrp_host_address=addr;
  lwrp_port=port;
  lwrp_password=pwd;
  lwrp_persistent=persistent;
  lwrp_socket->connectToHost(addr.toString(),port);
  if(lwrp_timeout_interval>0) {
    lwrp_timeout_timer->start(lwrp_timeout_interval);
  }
}


int SyLwrpClient::timeoutInterval() const
{
  return lwrp_timeout_interval;
}


void SyLwrpClient::setTimeoutInterval(int msec)
{
  lwrp_timeout_interval=msec;
}


void SyLwrpClient::sendRawLwrp(const QString &cmd)
{
  lwrp_socket->write((cmd+"\r\n").toUtf8());
}


void SyLwrpClient::close()
{
  lwrp_socket->close();
}


void SyLwrpClient::connectedData()
{
  QString cmd="LOGIN";

  lwrp_timeout_timer->stop();
  lwrp_connection_error=(QAbstractSocket::SocketError)-2;
  if(!lwrp_password.isEmpty()) {
    cmd+=" "+lwrp_password;
  }
  sendRawLwrp(cmd);
  sendRawLwrp("VER");
  lwrp_connection_timer->start(SWITCHYARD_LWRP_CONNECTION_TIMEOUT);
}


void SyLwrpClient::disconnectedData()
{
  // DEBUG
  //  printf("disconnectedData() - %s\n",(const char *)lwrp_host_address.toString().toUtf8());
}


void SyLwrpClient::errorData(QAbstractSocket::SocketError err)
{
  if(lwrp_connection_error!=err) {
    emit connectionError(lwrp_id,err);
    lwrp_connection_error=err;
  }
  if(lwrp_socket->state()==QAbstractSocket::ConnectedState) {
    lwrp_socket->disconnect();
    if(lwrp_connected) {
      lwrp_connected=false;
      emit connected(lwrp_id,false);
    }
  }
  if(lwrp_persistent) {
    lwrp_watchdog_retry_timer->stop();
    lwrp_watchdog_retry_timer->start(0);
  }
}


void SyLwrpClient::timeoutData()
{
  errorData(QAbstractSocket::SocketTimeoutError);
}


void SyLwrpClient::readyReadData()
{
  int n;
  char data[1500];

  while((n=lwrp_socket->read(data,1500))>0) {
    data[n]=0;
    //printf("%s: |%s|\n",(const char *)lwrp_host_address.toString().toUtf8(),
    //	   data);
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


void SyLwrpClient::connectionTimeoutData()
{
  if(lwrp_connection_error!=QAbstractSocket::SocketTimeoutError) {
    lwrp_connection_error=QAbstractSocket::SocketTimeoutError;
    emit connectionError(lwrp_id,QAbstractSocket::SocketTimeoutError);
    if(lwrp_persistent) {
      lwrp_watchdog_retry_timer->start(0);
    }
  }
}


void SyLwrpClient::watchdogIntervalData()
{
  //  fprintf(stderr,"sending watchdog\n");

  sendRawLwrp("VER");
  lwrp_watchdog_retry_timer->start(SWITCHYARD_LWRP_WATCHDOG_RETRY);
}


void SyLwrpClient::watchdogRetryData()
{
  //  fprintf(stderr,"watchdog failure detected!\n");

  //
  // Reset Connection
  //
  lwrp_socket->close();
  delete lwrp_socket;
  for(unsigned i=0;i<lwrp_sources.size();i++) {
    delete lwrp_sources[i];
  }
  lwrp_sources.clear();
  for(unsigned i=0;i<lwrp_destinations.size();i++) {
    delete lwrp_destinations[i];
  }
  lwrp_destinations.clear();
  for(int i=0;i<2;i++) {
    lwrp_source_clip_alarms[i].clear();
    lwrp_source_silence_alarms[i].clear();
    lwrp_destination_clip_alarms[i].clear();
    lwrp_destination_silence_alarms[i].clear();
  }
  for(unsigned i=0;i<lwrp_gpis.size();i++) {
    delete lwrp_gpis[i];
    delete lwrp_prev_gpi_timers[i];
  }
  lwrp_gpis.clear();
  lwrp_prev_gpi_timers.clear();
  for(unsigned i=0;i<lwrp_gpos.size();i++) {
    delete lwrp_gpos[i];
    delete lwrp_prev_gpo_timers[i];
  }
  lwrp_gpos.clear();
  lwrp_prev_gpi_timers.clear();

  lwrp_socket=new QTcpSocket(this);
  connect(lwrp_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(lwrp_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  connect(lwrp_socket,SIGNAL(error(QAbstractSocket::SocketError)),
	  this,SLOT(errorData(QAbstractSocket::SocketError)));
  if(lwrp_connected) {
    lwrp_connected=false;
    emit connected(lwrp_id,false);
  }
  if(lwrp_persistent) {
    connectToHost(lwrp_host_address,lwrp_port,lwrp_password,true);
  }
}


void SyLwrpClient::inputMeterData()
{
  if(lwrp_connected) {
    sendRawLwrp("MTR ICH");
  }
}


void SyLwrpClient::outputMeterData()
{
  if(lwrp_connected) {
    sendRawLwrp("MTR OCH");
  }
}


void SyLwrpClient::gpiResetData(int slotnum)
{
  QString cmd=QString::asprintf("GPI %d ",
				slotnum+1)+lwrp_prev_gpi_states[slotnum];
  sendRawLwrp(cmd);
}


void SyLwrpClient::gpoResetData(int slotnum)
{
  QString cmd=QString::asprintf("GPO %d ",
				slotnum+1)+lwrp_prev_gpo_states[slotnum];
  sendRawLwrp(cmd);
}


void SyLwrpClient::ProcessCommand(const QString &cmd)
{
  //  printf("ProcessCommand(|%s|)\n",(const char *)cmd.toUtf8());

  bool handled=false;
  QStringList f0=SyAString(cmd.trimmed()).split(" ","\"");

  if(f0[0]=="VER") {
    ProcessVER(f0);
    handled=true;
  }
  if(f0[0]=="SRC") {
    ProcessSRC(f0);
    handled=true;
  }
  if(f0[0]=="DST") {
    ProcessDST(f0);
    handled=true;
  }
  if(f0[0]=="GPI") {
    ProcessGPI(f0);
    handled=true;
  }
  if(f0[0]=="GPO") {
    ProcessGPO(f0);
    handled=true;
  }
  if(f0[0]=="CFG") {
    ProcessCFG(f0);
    handled=true;
  }
  if(f0[0]=="IFC") {
    ProcessIFC(f0);
    handled=true;
  }
  if(f0[0]=="IP") {
    ProcessIP(f0);
    handled=true;
  }
  if(f0[0]=="MTR") {
    ProcessMTR(f0);
    handled=true;
  }
  if(f0[0]=="LVL") {
    ProcessLVL(f0);
    handled=true;
  }
  if((f0[0]=="BEGIN")||(f0[0]=="END")) {
    handled=true;
  }
  if(f0[0]=="ERROR") {
    SySyslog(LOG_WARNING,QString("received error response from ")+
	     lwrp_host_address.toString()+": \""+cmd+"\"");
    handled=true;
  }
  if(!handled) {
    SySyslog(LOG_DEBUG,QString("unhandled LWRP response received from ")+
	     lwrp_host_address.toString()+": \""+cmd+"\"");
  }
}


void SyLwrpClient::ProcessVER(const QStringList &cmds)
{
  if(lwrp_connection_timer->isActive()) {  // Initial connection
    lwrp_connection_timer->stop();
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
	  for(int i=0;i<2;i++) {
	    lwrp_source_clip_alarms[i].push_back(false);
	    lwrp_source_silence_alarms[i].push_back(false);
	  }
	}
	lwrp_node->setSrcSlotQuantity(f2[0].toInt());
      }
      if(f1[0]=="NDST") {
	QStringList f2=f1[1].split("/");
	for(int i=0;i<f2[0].toInt();i++) {
	  lwrp_destinations.push_back(new SyDestination());
	  for(int i=0;i<2;i++) {
	    lwrp_destination_clip_alarms[i].push_back(false);
	    lwrp_destination_silence_alarms[i].push_back(false);
	  }
	}
	lwrp_node->setDstSlotQuantity(f2[0].toInt());
      }
      if(f1[0]=="NGPI") {
	for(int i=0;i<f1[1].toInt();i++) {
	  lwrp_gpis.push_back(new SyGpioBundle());
	  lwrp_prev_gpi_timers.push_back(new QTimer(this));
	  lwrp_prev_gpi_timers.back()->setSingleShot(true);
	  lwrp_prev_gpi_mapper->setMapping(lwrp_prev_gpi_timers.back(),i);
	  connect(lwrp_prev_gpi_timers.back(),SIGNAL(timeout()),
		  lwrp_prev_gpi_mapper,SLOT(map()));
	  lwrp_prev_gpi_states.push_back(QString());
	}
	lwrp_node->setGpiSlotQuantity(f1[1].toUInt());
      }
      if(f1[0]=="NGPO") {
	for(int i=0;i<f1[1].toInt();i++) {
	  lwrp_gpos.push_back(new SyGpo());
	  lwrp_prev_gpo_timers.push_back(new QTimer(this));
	  lwrp_prev_gpo_timers.back()->setSingleShot(true);
	  lwrp_prev_gpo_mapper->setMapping(lwrp_prev_gpo_timers.back(),i);
	  connect(lwrp_prev_gpo_timers.back(),SIGNAL(timeout()),
		  lwrp_prev_gpo_mapper,SLOT(map()));
	  lwrp_prev_gpo_states.push_back(QString());
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
      sendRawLwrp("SRC");
    }
    if(lwrp_node->dstSlotQuantity()>0) {
      sendRawLwrp("DST");
    }
    if(lwrp_node->gpiSlotQuantity()>0) {
      sendRawLwrp("ADD GPI");
    }
    if(lwrp_node->gpoSlotQuantity()>0) {
      sendRawLwrp("ADD GPO");
      sendRawLwrp("CFG GPO");
    }
    if((lwrp_socket->peerAddress().toIPv4Address()>>24)==127) {
      sendRawLwrp("IFC");
    }
    sendRawLwrp("IP");

    for(unsigned i=0;i<lwrp_sources.size();i++) {
      SySource *src=lwrp_sources[i];
      if(src->clipThreshold()!=0) {
	sendRawLwrp(QString::asprintf("LVL ICH %u CLIP.LEVEL:%d CLIP.TIME:%d",
				      i+1,src->clipThreshold(),
				      src->clipTimeout()));
      }
      if(src->silenceThreshold()!=0) {
	sendRawLwrp(QString::asprintf("LVL ICH %u LOW.LEVEL:%d LOW.TIME:%d",
				      i+1,src->silenceThreshold(),
				      src->silenceTimeout()));
      }
    }
    for(unsigned i=0;i<lwrp_destinations.size();i++) {
      SyDestination *dst=lwrp_destinations[i];
      if(dst->clipThreshold()!=0) {
	sendRawLwrp(QString::asprintf("LVL OCH %u CLIP.LEVEL:%d CLIP.TIME:%d",
				      i+1,dst->clipThreshold(),
				      dst->clipTimeout()));
      }
      if(dst->silenceThreshold()!=0) {
	sendRawLwrp(QString::asprintf("LVL OCH %u LOW.LEVEL:%d LOW.TIME:%d",
				      i+1,dst->silenceThreshold(),
				      dst->silenceTimeout()));
      }
    }
    lwrp_product_name=SyNode::productName(lwrp_node->deviceName(),
					  lwrp_node->gpiSlotQuantity(),
					  lwrp_node->gpoSlotQuantity());
    lwrp_node->setProductName(lwrp_product_name);
  }
  else {  // Watchdog response
    //    fprintf(stderr,"receiving watchdog\n");
    lwrp_watchdog_retry_timer->stop();
    lwrp_watchdog_interval_timer->start(GetWatchdogInterval());
  }
}


void SyLwrpClient::ProcessSRC(const QStringList &cmds)
{
  bool ok=false;

  unsigned slotnum=cmds[1].toUInt(&ok)-1;
  if(ok&&(slotnum<srcSlots())) {
    SySource *src=lwrp_sources[slotnum];
    src->setExists(true);
    for(int i=2;i<cmds.size();i++) {
      QStringList f1=SyAString(cmds[i]).split(":","\"");
      if(f1[0]=="NCHN") {
	src->setChannels(f1[1].toUInt());
      }
      if(f1[0]=="PSNM") {
	src->setName(f1[1].replace("\"",""));
      }
      if(f1[0]=="LABL") {
	src->setLabel(f1[1].replace("\"",""));
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
    dst->setExists(true);
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
	lwrp_gpis[slotnum]->setCode(cmds[2].toLower());
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
	lwrp_gpos[slotnum]->bundle()->setCode(cmds[2].toLower());
      }
    }
  }
}


void SyLwrpClient::ProcessCFG(const QStringList &cmds)
{
  if(cmds.size()>=3) {
    bool ok=false;
    unsigned slotnum=cmds[2].toUInt(&ok)-1;
    SyGpo *gpo=lwrp_gpos[slotnum];
    if(cmds.size()==3) {
      gpo->setSourceAddress(QHostAddress(),-1);
      return;
    }
    for(int i=3;i<cmds.size();i++) {
      QStringList f0=SyAString(cmds[i]).split(":","\"");
      if(f0.size()>=2) {
	if(f0[0]=="NAME") {
	  gpo->setName(f0[1]);
	}
	if(f0[0]=="SRCA") {
	  QStringList f1=f0[1].split(" ");
	  QString srca=f1[0];
	  srca.replace("\"","");
	  unsigned srcnum=srca.toUInt(&ok);
	  if(ok) {  // Source number
	    // FIXME: This breaks with surround sound!
	    gpo->setSourceAddress(SyRouting::streamAddress(SyRouting::Stereo,
							   srcnum),-1);
	  }
	  else {  // IP address
	    QStringList f2=srca.split("/");
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
  bool processed=false;

  //
  // 'Old-style' IP Payload
  //
  if(cmds.size()==9) {
    lwrp_hostname=cmds[8];
    lwrp_node->setHostName(cmds[8]);
    lwrp_host_address.setAddress(cmds.at(2));
    lwrp_node->setHostAddress(QHostAddress(cmds[2]));
    processed=true;
  }

  //
  // 'New-style' IP Payload
  //
  if(cmds.size()==3) {
    if(cmds.at(1).toLower()=="hostname") {
      lwrp_hostname=cmds.at(2);
      lwrp_node->setHostName(cmds.at(2));
      processed=true;
    }
    else {
      QStringList f0=cmds.at(1).split(":");
      if((f0.size()==2)&&(f0.at(0)=="ADDR")) {
	lwrp_host_address.setAddress(f0.at(1));
	lwrp_node->setHostAddress(QHostAddress(f0.at(1)));
      }
    }
  }

  if(processed) {
    lwrp_watchdog_state=true;
    lwrp_watchdog_interval_timer->start(GetWatchdogInterval());
    if(!lwrp_connected) {
      lwrp_connected=true;
      emit connected(lwrp_id,true);
    }
  }
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


void SyLwrpClient::ProcessMTR(const QStringList &cmds)
{
  QHostAddress addr;
  unsigned slotnum;
  bool ok=false;
  int16_t peak_lvls[SWITCHYARD_MAX_CHANNELS];
  int16_t rms_lvls[SWITCHYARD_MAX_CHANNELS]={1};
  QStringList f0;

  if((cmds.size()==4)||(cmds.size()==5)) {
    slotnum=cmds[2].toUInt(&ok)-1;
    if(ok) {
      for(int i=3;i<cmds.size();i++) {
	f0=cmds[3].split(":");
	if(f0.size()==3) {
	  if(f0[0].toLower()=="peek") {
	    for(int i=0;i<SWITCHYARD_MAX_CHANNELS;i++) {
	      peak_lvls[i]=f0[i+1].toInt();
	    }
	  }
	  if(f0[0].toLower()=="rms") {
	    for(int i=0;i<SWITCHYARD_MAX_CHANNELS;i++) {
	      rms_lvls[i]=f0[i+11].toInt();
	    }
	  }
	}
      }
      for(int i=0;i<SWITCHYARD_MAX_CHANNELS;i++) {
	if(rms_lvls[i]>0) {
	  rms_lvls[i]=peak_lvls[i];
	}
      }
      if(cmds[1]=="ICH") {
	emit meterUpdate(lwrp_id,SyLwrpClient::InputMeter,slotnum,
			 peak_lvls,rms_lvls);
      }
      if(cmds[1]=="OCH") {
	emit meterUpdate(lwrp_id,SyLwrpClient::OutputMeter,slotnum,
			 peak_lvls,rms_lvls);
      }
    }
  }
}


void SyLwrpClient::ProcessLVL(const QStringList &cmds)
{
  QStringList f0;
  QStringList f1;
  unsigned slotnum;
  SyLwrpClient::MeterType type=SyLwrpClient::OutputMeter;
  int chan=0;
  bool ok=false;

  if(cmds.size()==4) {
    if(cmds[1]=="ICH") {
      type=SyLwrpClient::InputMeter;
    }
    f0=cmds[2].split(".");
    if(f0.size()==2) {
      slotnum=f0[0].toInt(&ok)-1;
      if(ok) {
	if(f0[1]=="R") {
	  chan=1;
	}
	f1=cmds[3].split("-");
	if(f1[f1.size()-1]=="LOW") {
	  if(type==SyLwrpClient::InputMeter) {
	    lwrp_source_silence_alarms[chan][slotnum]=f1.size()==1;
	  }
	  else {
	    lwrp_destination_silence_alarms[chan][slotnum]=f1.size()==1;
	  }
	  emit audioSilenceAlarm(lwrp_id,type,slotnum,chan,f1.size()==1);
	}
	if(f1[f1.size()-1]=="CLIP") {
	  if(type==SyLwrpClient::InputMeter) {
	    lwrp_source_clip_alarms[chan][slotnum]=f1.size()==1;
	  }
	  else {
	    lwrp_destination_clip_alarms[chan][slotnum]=f1.size()==1;
	  }
	  emit audioClipAlarm(lwrp_id,type,slotnum,chan,f1.size()==1);
	}
      }
    }
  }
}


int SyLwrpClient::GetWatchdogInterval() const
{
  int interval=((double)random()/(double)RAND_MAX)*(SWITCHYARD_LWRP_WATCHDOG_INTERVAL_MAX-SWITCHYARD_LWRP_WATCHDOG_INTERVAL_MIN)+SWITCHYARD_LWRP_WATCHDOG_INTERVAL_MIN;
  //  fprintf(stderr,"watchdog interval: %d\n",interval);
  return interval;
}
