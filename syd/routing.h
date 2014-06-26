// routing.h
//
// Switchyard stream routing configuration
//
// (C) 2014 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <stdint.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <vector>

#include <QtCore/QString>
#include <QtNetwork/QHostAddress>

#include "config.h"

struct ShmBlock {
  uint32_t nic_addr;
  uint32_t src_addr[SWITCHYARD_SLOTS];
  uint32_t dst_addr[SWITCHYARD_SLOTS];
};

class Routing
{
 public:
  Routing();
  QHostAddress nicAddress() const;
  void setNicAddress(const QHostAddress &addr);
  QHostAddress srcAddress(int slot) const;
  void setSrcAddress(int slot,const QHostAddress &addr);
  QHostAddress dstAddress(int slot) const;
  void setDstAddress(int slot,const QHostAddress &addr);
  struct ShmBlock *shmBlock() const;
  unsigned nicQuantity() const;
  QHostAddress nicAddress(unsigned n);
  QString nicDevice(unsigned n);
  void load();
  void save() const;

 private:
  bool InitShmSegment(struct ShmBlock **sy_shm);
  void LoadInterfaces();
  int sy_shm_id;
  struct ShmBlock *sy_shm_block;
  std::vector<QHostAddress> sy_nic_addresses;
  std::vector<QString> sy_nic_devices;
};
