// adv_packet.h
//
// Abstract a LiveWire Control Protocol packet.
//
// (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: lwpacket.h,v 1.4 2010/10/12 22:38:07 pcvs Exp $
//
//   All Rights Reserved.
//

#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

#include <vector>

#include "adv_tag.h"

class Packet
{
 public:
  Packet();
  ~Packet();
  uint32_t sequenceNumber() const;
  void setSequenceNumber(uint32_t num);
  unsigned tags() const;
  Tag *tag(unsigned n);
  void addTag(const Tag &tag);
  bool readPacket(uint8_t *data,uint32_t size);
  int writePacket(uint8_t *data,uint32_t maxsize);
  QString dump() const;
  Packet &operator++();
  Packet operator++(int);

 private:
  uint32_t lw_sequence_number;
  std::vector<Tag *> lw_tags;
};


#endif  // PACKET_H
