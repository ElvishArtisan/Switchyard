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

#include <QMap>

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


QString SyNode::productName() const
{
  return node_product_name;
}


void SyNode::setProductName(const QString &str)
{
  node_product_name=str;
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
  ret+="productName: "+productName()+"\n";
  ret+="model: "+model()+"\n";
  ret+="softwareVersion: "+softwareVersion()+"\n";
  ret+="lwrpVersion: "+lwrpVersion()+"\n";
  ret+=QString().sprintf("srcSlotQuantity: %u\n",srcSlotQuantity());
  ret+=QString().sprintf("dstSlotQuantity: %u\n",dstSlotQuantity());
  ret+=QString().sprintf("gpiSlotQuantity: %u\n",gpiSlotQuantity());
  ret+=QString().sprintf("gpoSlotQuantity: %u\n",gpoSlotQuantity());

  return ret;
}


QString SyNode::productName(const QString &devn_str,int gpis,int gpos)
{
  QString ret;

  //
  // Check for weird "one-off" cases
  //
  if((devn_str.toLower()=="liveio")&&(gpis==8)&&(gpos==8)) {
    ret="AudioScience 6685 Card";
  }
  else {
    //
    // Otherwise, use the standard map
    //
    QMap<QString,QString> devn_map;
    devn_map["axiaalsa"]="Linux Host";
    devn_map["axiamacdr"]="MacOS Host";
    devn_map["axiarcp.xy"]="Crosspoint Route Controller";
    devn_map["axiarcp_xy"]="Crosspoint Route Controller";
    devn_map["axiaunode.mic"]="Microphone xNode";
    devn_map["axiaunknown"]="Livewire Device";
    devn_map["axiaunode_mic"]="Microphone xNode";
    devn_map["axiaxnode.aes4x4"]="AES3 xNode";
    devn_map["axiaxnode_aes4x4"]="AES3 xNode";
    devn_map["axiaxnode.analog4x4"]="Analog xNode";
    devn_map["axiaxnode_analog4x4"]="Analog xNode";
    devn_map["axiaxnode.gpio"]="GPIO xNode";
    devn_map["axiaxnode_gpio"]="GPIO xNode";
    devn_map["axiaxnode.mic"]="Microphone xNode";
    devn_map["axiaxnode_mic"]="Microphone xNode";
    devn_map["axiaxnode.router"]="Router xNode";
    devn_map["axiaxnode_router"]="Router xNode";
    devn_map["axiaxnode.sdi"]="SDI xNode";
    devn_map["axiaxnode_sdi"]="SDI xNode";
    devn_map["axiaxnode.swps"]="xNode Ethernet Switch";
    devn_map["axiaxnode_swps"]="xNode Ethernet Switch";
    devn_map["axiaxnode.combo"]="Combination xNode";
    devn_map["axiaxnode_combo"]="Combination xNode";
    devn_map["axiaxnodexsdi"]="SDI xNode";
    devn_map["element"]="Element Control Surface";
    devn_map["engine"]="Mix Engine (Gen1)";
    devn_map["engineacl"]="Blade Engine";
    devn_map["eth4can"]="CAN Bus Bridge";
    devn_map["evertz"]="Evertz Router";
    devn_map["fusion"]="Fusion Control Surface";
    devn_map["hx6"]="Telos HX6 Phone Hybrid";
    devn_map["iceio"]="PowerStation Node";
    devn_map["imaginelrcrouter"]="Imagine Lrc Router";
    devn_map["infinity"]="Infinity Device";
    devn_map["infinitybelt"]="Infinity Belt Pack";
    devn_map["infinityds16"]="Infinity 16 Button Panel (Desktop)";
    devn_map["infinitymp16"]="Infinity 16 Button Panel";
    devn_map["ipcom"]="Axia Intercom";
    devn_map["irouter"]="Paravel Systems iRoute";
    devn_map["liveaes"]="AES Node (Gen1)";
    devn_map["gpio"]="GPIO Node (Gen1)";
    devn_map["hp202"]="SLForge HP202";
    devn_map["iq6"]="IQ6 Control Surface";
    devn_map["iqx"]="IQX Control Surface";
    devn_map["liveio"]="Analog Node (Gen1)"; // OR an ASI6x85 card!
    devn_map["livemic"]="Microphone Node (Gen1)";
    devn_map["livert"]="Router Node (Gen1)";
    devn_map["lwwd"]="Windows Host";
    devn_map["nodeemulator"]="Node Emulator";
    devn_map["nti"]="NTI VGA Router";
    devn_map["omnia_one"]="Omnia One Processor";
    devn_map["omnia.one"]="Omnia One Processor";
    devn_map["omnia.volt"]="Omnia Volt Processor";
    devn_map["omnia_volt"]="Omnia Volt Processor";
    devn_map["omnia8x"]="Omnia 8x Processor";
    devn_map["pathfinder"]="Pathfinder Core";
    devn_map["pcp.gpio"]="Pathfinder PRO GPIO Node";
    devn_map["pcp_gpio"]="Pathfinder PRO GPIO Node";
    devn_map["pdm257"]="25-Seven Program Delay Manager";
    devn_map["prostream"]="Telos Prostream";
    devn_map["prostrm"]="Telos Pro Stream";
    devn_map["ps.engine"]="PowerStation";
    devn_map["ps_engine"]="PowerStation";
    devn_map["qengine"]="Quasar Engine";
    devn_map["qor"]="QOR Engine";
    devn_map["quasar"]="Quasar Control Surface";
    devn_map["rcp17ss"]="17 Button Smart Switch LCD Panel";
    devn_map["rcp8oled"]="8 Button OLED Panel";
    devn_map["rcp9ss"]="9 Button Smart Switch LCD Panel";
    devn_map["rcpacc"]="1 OLED Accessory Panel";
    devn_map["restapi"]="R.E.S.T. API";
    devn_map["saport"]="SAPort Router";
    devn_map["sigma"]="Sigma Router";
    devn_map["sound4streamer"]="Sound 4 Voco 8";
    devn_map["tcpserialhub"]="Serial Hub";
    devn_map["tlsintercom"]="Infinity Device";
    devn_map["tlsintercom.bp-2"]="Infinity Belt Pack";
    devn_map["tlsintercom_bp-2"]="Infinity Belt Pack";
    devn_map["tlsintercom.mp-16"]="Infinity 16 Button Panel";
    devn_map["tlsintercom_mp-16"]="Infinity 16 Button Panel";
    devn_map["videoquip"]="VideoQuip DS1224 Router";
    devn_map["vx.engine"]="VX Phone Engine";
    devn_map["vx_engine"]="VX Phone Engine";
    devn_map["vx engine"]="VX Phone Engine";
    devn_map["xiaudio"]="XI Audio UDP Router";
    devn_map["xnode.sdi"]="SDI xNode";
    devn_map["xnode_sdi"]="SDI xNode";
    devn_map["zipone"]="Telos Z/ip One";
    devn_map["ziport"]="Axia iPort";
    devn_map["zsys64"]="Z-Systems 64x64 Detangler";
    devn_map["zsys128"]="Z-Systems 128x128 Detangler";
    devn_map["zxs2"]="Telos Zeyphr XStream";

    ret=devn_map.value(devn_str.toLower(),"Livewire Device ["+devn_str+"]");
  }

  return ret;
}
