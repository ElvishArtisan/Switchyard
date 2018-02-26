// syadv_packet.h
//
// Abstract a LiveWire Control Protocol packet.
//
// (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
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
