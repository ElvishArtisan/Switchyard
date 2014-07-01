// routing.h
//
// AoIP stream routing configuration
//
// (C) 2014 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#ifndef ROUTING_H
#define ROUTING_H

#include <stdint.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <vector>

#include <QtCore/QString>
#include <QtNetwork/QHostAddress>

#include "config.h"

class Routing
{
 public:
  Routing(unsigned d_slots,unsigned s_slots);
  unsigned dstSlots() const;
  unsigned srcSlots() const;
  QString hostName() const;
  QHostAddress nicAddress() const;
  void setNicAddress(const QHostAddress &addr);
  QHostAddress nicNetmask() const;
  void setNicNetmask(const QHostAddress &addr);
  QHostAddress clkAddress() const;
  void setClkAddress(const QHostAddress &addr);
  int srcNumber(int slot) const;
  QHostAddress srcAddress(int slot) const;
  void setSrcAddress(int slot,const QHostAddress &addr);
  void setSrcAddress(int slot,const QString &addr);
  QString srcName(int slot) const;
  void setSrcName(int slot,const QString &str);
  bool srcEnabled(int slot) const;
  void setSrcEnabled(int slot,bool state);
  int activeSources() const;
  QHostAddress dstAddress(int slot) const;
  void setDstAddress(int slot,const QHostAddress &addr);
  void setDstAddress(int slot,const QString &addr);
  QString dstName(int slot) const;
  void setDstName(int slot,const QString &str);
  unsigned nicQuantity() const;
  QHostAddress nicAddress(unsigned n);
  QHostAddress nicNetmask(unsigned n);
  QString nicDevice(unsigned n);
  void subscribe(const QHostAddress &addr);
  void unsubscribe(const QHostAddress &addr);
  void load();
  void save() const;
  uint32_t nic_addr;
  uint32_t nic_mask;
  uint32_t clk_addr;
  uint32_t src_slots;
  uint32_t dst_slots;
  uint32_t src_addr[SWITCHYARD_MAX_SLOTS];
  bool src_enabled[SWITCHYARD_MAX_SLOTS];
  uint32_t dst_addr[SWITCHYARD_MAX_SLOTS];
  static QString dumpAddress(uint32_t addr);

 private:
  void LoadInterfaces();
  QString sy_src_names[SWITCHYARD_MAX_SLOTS];
  QString sy_dst_names[SWITCHYARD_MAX_SLOTS];
  std::vector<QHostAddress> sy_nic_addresses;
  std::vector<QHostAddress> sy_nic_netmasks;
  std::vector<QString> sy_nic_devices;
  int sy_fd;
};


#endif  // ROUTING_H
