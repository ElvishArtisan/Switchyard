// syinterfaces.h
//
// Network interface information
//
// (C) 2017 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#ifndef SYINTERFACES_H
#define SYINTERFACES_H

#include <stdint.h>

#include <QHostAddress>
#include <QList>
#include <QString>
#include <QStringList>

#include <sy/syendpoint.h>

class SyInterfaces
{
 public:
  SyInterfaces();
  int quantity() const;
  QString name(int n) const;
  uint64_t macAddress(int n);
  QHostAddress ipv4Address(int n) const;
  QHostAddress ipv4Netmask(int n) const;
  bool update();
  static QString macString(uint64_t maddr);

 private:
  QStringList iface_names;
  QList<uint64_t> iface_mac_addresses;
  QList<QHostAddress> iface_ipv4_addresses;
  QList<QHostAddress> iface_ipv4_netmasks;
};


#endif  // SYINTERFACES_H
