// syrouting.h
//
// AoIP stream routing configuration
//
// (C) 2014 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYROUTING_H
#define SYROUTING_H

#include <math.h>
#include <stdint.h>

#include <vector>

#include <QHostAddress>
#include <QString>

#include <sy/syconfig.h>

class SyRouting
{
 public:
  enum Realm {Stereo=0,Backfeed=1,Surround=2};
  SyRouting(unsigned d_slots,unsigned s_slots,
	    unsigned gpis=0,unsigned gpos=0);
  unsigned dstSlots() const;
  unsigned srcSlots() const;
  unsigned gpis() const;
  unsigned gpos() const;
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
  int srcMeterLevel(int slot,int chan) const;
  int activeSources() const;
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
  QHostAddress gpoAddress(int slot) const;
  void setGpoAddress(int slot,const QHostAddress &addr);
  void setGpoAddress(int slot,const QString &addr);
  QString gpoName(int slot) const;
  void setGpoName(int slot,const QString &str);
  unsigned nicQuantity() const;
  QHostAddress nicAddress(unsigned n);
  QHostAddress nicNetmask(unsigned n);
  QString nicDevice(unsigned n);
  void subscribe(const QHostAddress &addr);
  void unsubscribe(const QHostAddress &addr);
  int subscriptionSocket() const;
  int rtpSendSocket() const;
  void load();
  void save() const;
  uint32_t nic_addr;
  uint32_t nic_mask;
  uint32_t clk_addr;
  uint32_t src_slots;
  uint32_t dst_slots;
  uint32_t src_addr[SWITCHYARD_MAX_SLOTS];
  bool src_enabled[SWITCHYARD_MAX_SLOTS];
  float src_meter[SWITCHYARD_MAX_SLOTS][SWITCHYARD_MAX_CHANNELS];
  uint32_t dst_addr[SWITCHYARD_MAX_SLOTS];
  float dst_meter[SWITCHYARD_MAX_SLOTS][SWITCHYARD_MAX_CHANNELS];
  uint32_t gpo_addr[SWITCHYARD_MAX_SLOTS];
  void writeRtpData(unsigned src_slot,const char *data,int len) const;
  static unsigned livewireNumber(const QHostAddress &addr);
  static QHostAddress streamAddress(Realm realm,uint16_t lw_num);
  static QString sourceString(const QHostAddress &s_addr,int s_slot);
  static QString dumpAddress(uint32_t addr);
  static QString socketErrorString(const QString &msg);

 private:
  int GetSlotByGpio(int gpio) const;
  void LoadInterfaces();
  QString sy_src_names[SWITCHYARD_MAX_SLOTS];
  QString sy_dst_names[SWITCHYARD_MAX_SLOTS];
  QString sy_gpo_names[SWITCHYARD_MAX_SLOTS];
  std::vector<bool> sy_gpi_states;
  std::vector<bool> sy_gpo_states;
  std::vector<QHostAddress> sy_nic_addresses;
  std::vector<QHostAddress> sy_nic_netmasks;
  std::vector<QString> sy_nic_devices;
  int sy_subscription_socket;
  int sy_rtp_send_socket;
};


#endif  // SYROUTING_H
