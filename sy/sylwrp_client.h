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

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>

#include <sy/syconfig.h>
#include <sy/sydestination.h>
#include <sy/sysource.h>
#include <sy/syrouting.h>

class SyLwrpClient :public QObject
{
  Q_OBJECT;
 public:
  SyLwrpClient(QObject *parent=0);
  ~SyLwrpClient();
  QString deviceName() const;
  unsigned dstSlots() const;
  unsigned srcSlots() const;
  unsigned gpis() const;
  unsigned gpos() const;
  QString hostName() const;
  uint16_t port() const;
  int srcNumber(int slot) const;
  QHostAddress srcAddress(int slot) const;
  void setSrcAddress(int slot,const QHostAddress &addr);
  void setSrcAddress(int slot,const QString &addr);
  QString srcName(int slot) const;
  void setSrcName(int slot,const QString &str);
  bool srcEnabled(int slot) const;
  void setSrcEnabled(int slot,bool state);
  int srcMeterLevel(int slot,int chan) const;
  QHostAddress dstAddress(int slot) const;
  void setDstAddress(int slot,const QHostAddress &addr);
  void setDstAddress(int slot,const QString &addr);
  QString dstName(int slot) const;
  void setDstName(int slot,const QString &str);
  int dstMeterLevel(int slot,int chan) const;
  bool gpiState(int gpi,int line) const;
  bool gpiStateBySlot(int slot,int line) const;
  void setGpi(int gpi,int line,bool state,bool pulse);
  bool gpoState(int gpo,int line) const;
  bool gpoStateBySlot(int slot,int line) const;
  void setGpo(int gpo,int line,bool state,bool pulse);
  void connectToHost(const QString &hostname,uint16_t port,const QString &pwd);

 signals:
  void connected();
  void sourceChanged(int slotnum,const SySource *src);
  void destinationChanged(int slotnum,const SyDestination *dst);

 private slots:
  void connectedData();
  void readyReadData();

 private:
  void SendCommand(const QString &cmd);
  void ProcessCommand(const QString &cmd);
  void ProcessVER(const QStringList &cmds);
  void ProcessSRC(const QStringList &cmds);
  void ProcessDST(const QStringList &cmds);
  void ProcessIP(const QStringList &cmds);
  std::vector<SySource *> lwrp_sources;
  std::vector<SyDestination *> lwrp_destinations;
  QString lwrp_hostname;
  uint16_t lwrp_port;
  QString lwrp_password;
  QTcpSocket *lwrp_socket;
  QString lwrp_buffer;
  QString lwrp_device_name;
  unsigned lwrp_gpis;
  unsigned lwrp_gpos;
  bool lwrp_connected;
};


#endif  // SYLWRP_CLIENT_H
