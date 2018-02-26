// synode.cpp
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

#include "synode.h"

SyNode::SyNode()
{
  node_src_slots=0;
  node_dst_slots=0;
  node_gpi_slots=0;
  node_gpo_slots=0;
}


QHostAddress SyNode::hostAddress() const
{
  return node_host_address;
}


void SyNode::setHostAddress(const QHostAddress &addr)
{
  node_host_address=addr;
}


QString SyNode::hostName() const
{
  return node_host_name;
}


void SyNode::setHostName(const QString &str)
{
  node_host_name=str;
}


QString SyNode::deviceName() const
{
  return node_device_name;
}


void SyNode::setDeviceName(const QString &str)
{
  node_device_name=str;
}


QString SyNode::product() const
{
  return node_product;
}


void SyNode::setProduct(const QString &str)
{
  node_product=str;
}


QString SyNode::model() const
{
  return node_model;
}


void SyNode::setModel(const QString &str)
{
  node_model=str;
}


QString SyNode::softwareVersion() const
{
  return node_software_version;
}


void SyNode::setSoftwareVersion(const QString &str)
{
  node_software_version=str;
}


QString SyNode::lwrpVersion() const
{
  return node_lwrp_version;
}


void SyNode::setLwrpVersion(const QString &str)
{
  node_lwrp_version=str;
}


unsigned SyNode::srcSlotQuantity() const
{
  return node_src_slots;
}


void SyNode::setSrcSlotQuantity(unsigned slot_quan)
{
  node_src_slots=slot_quan;
}


unsigned SyNode::dstSlotQuantity() const
{
  return node_dst_slots;
}


void SyNode::setDstSlotQuantity(unsigned slot_quan)
{
  node_dst_slots=slot_quan;
}


unsigned SyNode::gpiSlotQuantity() const
{
  return node_gpi_slots;
}


void SyNode::setGpiSlotQuantity(unsigned slot_quan)
{
  node_gpi_slots=slot_quan;
}


unsigned SyNode::gpoSlotQuantity() const
{
  return node_gpo_slots;
}


void SyNode::setGpoSlotQuantity(unsigned slot_quan)
{
  node_gpo_slots=slot_quan;
}


QString SyNode::dump() const
{
  QString ret="";

  ret+="hostAddress: "+hostAddress().toString()+"\n";
  ret+="hostName: "+hostName()+"\n";
  ret+="deviceName: "+deviceName()+"\n";
  ret+="product: "+product()+"\n";
  ret+="model: "+model()+"\n";
  ret+="softwareVersion: "+softwareVersion()+"\n";
  ret+="lwrpVersion: "+lwrpVersion()+"\n";
  ret+=QString().sprintf("srcSlotQuantity: %u\n",srcSlotQuantity());
  ret+=QString().sprintf("dstSlotQuantity: %u\n",dstSlotQuantity());
  ret+=QString().sprintf("gpiSlotQuantity: %u\n",gpiSlotQuantity());
  ret+=QString().sprintf("gpoSlotQuantity: %u\n",gpoSlotQuantity());

  return ret;
}
