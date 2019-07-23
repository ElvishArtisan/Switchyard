// syinterfaces.h
//
// Network interface information
//
// (C) 2017-2019 Fred Gleason <fredg@paravelsystems.com>
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
  static uint32_t toCidrMask(const QHostAddress &mask);
  static QHostAddress fromCidrMask(uint32_t mask);

 private:
  QStringList iface_names;
  QList<uint64_t> iface_mac_addresses;
  QList<QHostAddress> iface_ipv4_addresses;
  QList<QHostAddress> iface_ipv4_netmasks;
};


#endif  // SYINTERFACES_H
