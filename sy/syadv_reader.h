// syadv_reader.h
//
// Reader for AoIP Stream List
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#ifndef SYADV_READER_H
#define SYADV_READER_H

#include <vector>

#include <QHostAddress>
#include <QString>

#include "syconfig.h"

class SyAdvReader
{
 public:
  SyAdvReader();
  unsigned srcQuantity() const;
  unsigned slot(unsigned n) const;
  QHostAddress nodeAddress(unsigned n) const;
  QString nodeName(unsigned n) const;
  QHostAddress streamAddress(unsigned n) const;
  QString sourceName(unsigned n) const;
  QString fullName(unsigned n) const;
  bool load();
  void clear();
  QString dump() const;

 private:
  std::vector<unsigned> adv_slots;
  std::vector<QHostAddress> adv_node_addresses;
  std::vector<QString> adv_node_names;
  std::vector<QHostAddress> adv_stream_addresses;
  std::vector<QString> adv_source_names;
};


#endif  // SYADV_READER_H
