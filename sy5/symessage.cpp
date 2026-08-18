// symessage.cpp
//
// Abstract base class for messages.
//
// (C) Copyright 2026 Fred Gleason <fredg@paravelsystems.com>
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

#include "symessage.h"

SyMessage::SyMessage(const QNetworkDatagram &dgram)
{
  d_body=dgram.data();
  d_host_address=dgram.senderAddress();
}


SyMessage::SyMessage()
{
}


QByteArray SyMessage::body() const
{
  return d_body;
}


QHostAddress SyMessage::hostAddress() const
{
  return d_host_address;
}


void SyMessage::setHostAddress(const QHostAddress &addr)
{
  d_host_address=addr;
}


QString SyMessage::dumpBody()
{
  return toAscii(body());
}


QString SyMessage::toAscii()
{
  return SyMessage::toAscii(d_body);
}


QString SyMessage::toAscii(const QByteArray &data)
{
  QString ret="";
  QString hex;
  QString str;

  for(int i=0;i<data.size();i+=16) {
    ret+=QString::asprintf("%4d:",i);
    for(int j=0;j<8;j++) {
      if((i+j)<data.size()) {
	if((data.at(i+j)>31)&&(data.at(i+j)<177)) {
	  str+=data.at(i+j);
	}
	else {
	  str+=".";
	}
	hex+=QString::asprintf("%02X ",0xFF&data.at(i+j));
      }
    }
    hex+=" ";
    str+="  ";
    for(int j=8;j<16;j++) {
      if((i+j)<data.size()) {
	if((data.at(i+j)>31)&&(data.at(i+j)<177)) {
	  str+=data.at(i+j);
	}
	else {
	  str+=".";
	}
	hex+=QString::asprintf("%02X ",0xFF&data.at(i+j));
      }
    }
    ret+=hex;
    for(int j=0;j<(50-hex.length());j++) {
      ret+=" ";
    }
    hex="";
    ret+=str;
    str="";
    ret+="\n";
  }
  return ret;
}
