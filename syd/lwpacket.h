// lwpacket.h
//
// Abstract a LiveWire Control Protocol packet.
//
// (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: lwpacket.h,v 1.4 2010/10/12 22:38:07 pcvs Exp $
//
//   All Rights Reserved.
//

#ifndef LWPACKET_H
#define LWPACKET_H

#include <stdint.h>

#include <vector>

#include <lwtag.h>

class LwPacket
{
 public:
  LwPacket();
  ~LwPacket();
  uint32_t sequenceNumber() const;
  void setSequenceNumber(uint32_t num);
  unsigned tags() const;
  LwTag *tag(unsigned n);
  void addTag(const LwTag &tag);
  bool readPacket(uint8_t *data,uint32_t size);
  int writePacket(uint8_t *data,uint32_t maxsize);
  QString dump() const;
  LwPacket &operator++();
  LwPacket operator++(int);

 private:
  uint32_t lw_sequence_number;
  std::vector<LwTag *> lw_tags;
};


#endif  // LWPACKET_H
