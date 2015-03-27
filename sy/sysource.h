// sysource.h
//
// Container class for LiveWire source attributes
//
// (C) 2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#ifndef SYSOURCE_H
#define SYSOURCE_H

#include <QtCore/QString>
#include <QtNetwork/QHostAddress>

class SySource
{
 public:
  SySource(const QHostAddress &s_addr,const QString &name,bool enabled);
  SySource();
  QHostAddress streamAddress() const;
  void setStreamAddress(const QHostAddress &addr);
  QString name() const;
  void setName(const QString &name);
  bool enabled() const;
  void setEnabled(bool state);
  unsigned channels() const;
  void setChannels(unsigned chans);
  unsigned packetSize() const;
  void setPacketSize(unsigned size);
  bool shareable() const;
  void setShareable(bool state);

 private:
  QHostAddress src_stream_address;
  QString src_name;
  bool src_enabled;
  unsigned src_channels;
  unsigned src_packet_size;
  bool src_shareable;
};


#endif  // SYSOURCE_H
