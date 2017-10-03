// sylwrp_client.h
//
// LWRP client implementation
//
// (C) 2014-2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#ifndef SYLWRP_CLIENT_H
#define SYLWRP_CLIENT_H

#include <stdint.h>

#include <vector>

#include <QHostAddress>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QTimer>

#include <sy/syconfig.h>
#include <sy/sydestination.h>
#include <sy/sygpiobundle.h>
#include <sy/sygpo.h>
#include <sy/synode.h>
#include <sy/sysource.h>
#include <sy/syrouting.h>

#define SYLWRP_CLIENT_METER_INTERVAL 100

class SyLwrpClient :public QObject
{
  Q_OBJECT;
 public:
  enum MeterType {InputMeter=0,OutputMeter=1,LastTypeMeter=2};
  SyLwrpClient(unsigned id,QObject *parent=0);
  ~SyLwrpClient();
  unsigned id() const;
  bool isConnected() const;
  SyNode *node() const;
  QString deviceName() const;
  unsigned dstSlots() const;
  unsigned srcSlots() const;
  SySource *src(int slot) const;
  SyDestination *dst(int slot) const;
  unsigned gpis() const;
  unsigned gpos() const;
  QString hostName() const;
  QHostAddress hostAddress() const;
  uint16_t port() const;
  int srcNumber(int slot) const;
  QHostAddress srcAddress(int slot) const;
  void setSrcAddress(int slot,const QHostAddress &addr);
  void setSrcAddress(int slot,const QString &addr);
  QString srcName(int slot) const;
  void setSrcName(int slot,const QString &str);
  QString srcLabel(int slot) const;
  void setSrcLabel(int slot,const QString &str);
  bool srcEnabled(int slot) const;
  void setSrcEnabled(int slot,bool state);
  unsigned srcChannels(int slot) const;
  void setSrcChannels(int slot,unsigned chans);
  unsigned srcPacketSize(int slot);
  void setSrcPacketSize(int slot,unsigned size);
  bool srcShareable(int slot) const;
  void setSrcShareable(int slot,bool state);
  int srcMeterLevel(int slot,int chan) const;
  QHostAddress dstAddress(int slot) const;
  void setDstAddress(int slot,const QHostAddress &addr);
  void setDstAddress(int slot,const QString &addr);
  QString dstName(int slot) const;
  void setDstName(int slot,const QString &str);
  unsigned dstChannels(int slot) const;
  void setDstChannels(int slot,unsigned chans);
  int dstMeterLevel(int slot,int chan) const;
  SyGpioBundle *gpiBundle(int slot) const;
  void setGpiCode(int slot,const QString &code);
  SyGpo *gpo(int slot) const;
  void setGpoCode(int slot,const QString &code);
  void setGpoName(int slot,const QString &str);
  void setGpoSourceAddress(int slot,const QHostAddress &s_addr,int s_slot);
  void setGpoFollow(int slot,bool state);
  void setClipMonitor(int slot,MeterType type,int lvl,int msec);
  void setSilenceMonitor(int slot,MeterType type,int lvl,int msec);
  void startMeter(MeterType type);
  void stopMeter(MeterType type);
  QHostAddress nicAddress() const;
  void setNicAddress(const QHostAddress &addr);
  void connectToHost(const QHostAddress &addr,uint16_t port,const QString &pwd,
		     bool persistent=false);
  int timeoutInterval() const;
  void setTimeoutInterval(int msec);
  void close();

 signals:
  void connected(unsigned id,bool state);
  void connectionError(unsigned id,QAbstractSocket::SocketError err);
  void sourceChanged(unsigned id,int slotnum,const SyNode &node,
		     const SySource &src);
  void destinationChanged(unsigned id,int slotnum,const SyNode &node,
			  const SyDestination &dst);
  void gpiChanged(unsigned id,int slotnum,const SyNode &node,
		  const SyGpioBundle &bundle);
  void gpoChanged(unsigned id,int slotnum,const SyNode &node,const SyGpo &gpo);
  void nicAddressChanged(unsigned id,const QHostAddress &nicaddr);
  void meterUpdate(unsigned id,SyLwrpClient::MeterType type,unsigned slotnum,
		   int16_t *peak_lvls,int16_t *rms_lvls);
  void audioClipAlarm(unsigned id,SyLwrpClient::MeterType type,
		      unsigned slotnum,int chan,bool state);
  void audioSilenceAlarm(unsigned id,SyLwrpClient::MeterType type,
			 unsigned slotnum,int chan,bool state);

 private slots:
  void connectedData();
  void disconnectedData();
  void errorData(QAbstractSocket::SocketError err);
  void timeoutData();
  void readyReadData();
  void connectionTimeoutData();
  void watchdogIntervalData();
  void watchdogRetryData();
  void inputMeterData();
  void outputMeterData();

 private:
  void SendCommand(const QString &cmd);
  void ProcessCommand(const QString &cmd);
  void ProcessVER(const QStringList &cmds);
  void ProcessSRC(const QStringList &cmds);
  void ProcessDST(const QStringList &cmds);
  void ProcessGPI(const QStringList &cmds);
  void ProcessGPO(const QStringList &cmds);
  void ProcessCFG(const QStringList &cmds);
  void ProcessIP(const QStringList &cmds);
  void ProcessIFC(const QStringList &cmds);
  void ProcessMTR(const QStringList &cmds);
  void ProcessLVL(const QStringList &cmds);
  int GetWatchdogInterval() const;
  std::vector<SySource *> lwrp_sources;
  std::vector<SyDestination *> lwrp_destinations;
  std::vector<SyGpioBundle *> lwrp_gpis;
  std::vector<SyGpo *> lwrp_gpos;
  SyNode *lwrp_node;
  QHostAddress lwrp_host_address;
  QString lwrp_hostname;
  uint16_t lwrp_port;
  QString lwrp_password;
  bool lwrp_persistent;
  QTcpSocket *lwrp_socket;
  QString lwrp_buffer;
  QString lwrp_device_name;
  bool lwrp_connected;
  QHostAddress lwrp_nic_address;
  unsigned lwrp_id;
  QTimer *lwrp_connection_timer;
  QTimer *lwrp_watchdog_interval_timer;
  QTimer *lwrp_watchdog_retry_timer;
  bool lwrp_watchdog_state;
  QAbstractSocket::SocketError lwrp_connection_error;
  QTimer *lwrp_meter_timers[SyLwrpClient::LastTypeMeter];
  int lwrp_timeout_interval;
  QTimer *lwrp_timeout_timer;
};


#endif  // SYLWRP_CLIENT_H
