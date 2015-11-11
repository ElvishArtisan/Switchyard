// sygpo.h
//
// Abstract a LiveWire GPO slot
//
// (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#ifndef SYGPO_H
#define SYGPO_H

#include <QHostAddress>
#include <QString>

#include <sy/sygpiobundle.h>

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
