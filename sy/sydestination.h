// sydestination.h
//
// Container class for LiveWire destination attributes
//
// (C) 2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#ifndef SYDESTINATION_H
#define SYDESTINATION_H

#include <QtCore/QString>
#include <QtNetwork/QHostAddress>

class SyDestination
{
 public:
  SyDestination(const QHostAddress &s_addr,const QString &name);
  SyDestination();
  QHostAddress streamAddress() const;
  void setStreamAddress(const QHostAddress &addr);
  QString name() const;
  void setName(const QString &name);

 private:
  QHostAddress dst_stream_address;
  QString dst_name;
};


#endif  // SYDESTINATION_H
