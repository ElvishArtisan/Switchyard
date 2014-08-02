// syadv_packet.h
//
// Abstract a LiveWire Control Protocol packet.
//
// (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: lwpacket.h,v 1.4 2010/10/12 22:38:07 pcvs Exp $
//
//   All Rights Reserved.
//

#ifndef SYADV_PACKET_H
#define SYADV_PACKET_H

#include <stdint.h>

#include <vector>

#include <sy/syadv_tag.h>

class SyAdvPacket
{
 public:
  SyAdvPacket();
  ~SyAdvPacket();
  uint32_t sequenceNumber() const;
  void setSequenceNumber(uint32_t num);
  unsigned tags() const;
  SyTag *tag(unsigned n);
  void addTag(const SyTag &tag);
  bool readPacket(uint8_t *data,uint32_t size);
  int writePacket(uint8_t *data,uint32_t maxsize);
  QString dump() const;
  SyAdvPacket &operator++();
  SyAdvPacket operator++(int);

 private:
  uint32_t lw_sequence_number;
  std::vector<SyTag *> lw_tags;
};


#endif  // SYADV_PACKET_H
