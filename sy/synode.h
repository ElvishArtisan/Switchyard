// synode.h
//
// Container class for LiveWire node attributes
//
// (C) 2015 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYNODE_H
#define SYNODE_H

#include <QHostAddress>
#include <QString>

class SyNode
{
 public:
  SyNode();
  QHostAddress hostAddress() const;
  void setHostAddress(const QHostAddress &addr);
  QString hostName() const;
  void setHostName(const QString &str);
  QString deviceName() const;
  void setDeviceName(const QString &str);
  QString product() const;
  void setProduct(const QString &str);
  QString model() const;
  void setModel(const QString &str);
  QString softwareVersion() const;
  void setSoftwareVersion(const QString &str);
  QString lwrpVersion() const;
  void setLwrpVersion(const QString &str);
  unsigned srcSlotQuantity() const;
  void setSrcSlotQuantity(unsigned slot_quan);
  unsigned dstSlotQuantity() const;
  void setDstSlotQuantity(unsigned slot_quan);
  unsigned gpiSlotQuantity() const;
  void setGpiSlotQuantity(unsigned slot_quan);
  unsigned gpoSlotQuantity() const;
  void setGpoSlotQuantity(unsigned slot_quan);
  QString dump() const;

 private:
  QHostAddress node_host_address;
  QString node_host_name;
  QString node_device_name;
  QString node_product;
  QString node_model;
  QString node_software_version;
  QString node_lwrp_version;
  unsigned node_src_slots;
  unsigned node_dst_slots;
  unsigned node_gpi_slots;
  unsigned node_gpo_slots;
};


#endif  // SYNODE_H
