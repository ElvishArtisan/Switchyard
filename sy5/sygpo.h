// sygpo.h
//
// Abstract a LiveWire GPO slot
//
// (C) Copyright 2015-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYGPO_H
#define SYGPO_H

#include <QHostAddress>
#include <QString>

#include <sy5/sygpiobundle.h>

class SyGpo
{
 public:
  SyGpo();
  ~SyGpo();
  QString name() const;
  void setName(const QString &str);
  QHostAddress sourceAddress() const;
  int sourceSlot() const;
  void setSourceAddress(const QHostAddress &s_addr,int s_slot);
  bool follow() const;
  void setFollow(bool state);
  SyGpioBundle *bundle() const;

 private:
  QString gpo_name;
  QHostAddress gpo_source_address;
  int gpo_source_slot;
  bool gpo_follow;
  SyGpioBundle *gpo_bundle;
};


#endif  // SYGPO_H
