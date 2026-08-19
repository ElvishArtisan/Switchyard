// syhowdy_message.cpp
//
// Abstract a WheatNet HOWDY mMssage
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

#include <QObject>

#include "syhowdy_message.h"

SyHowdyMessage::SyHowdyMessage(const QNetworkDatagram &dgram)
  : SyMessage(dgram)
{
  QByteArray line;
  int istate=0;
  for(int i=40;i<dgram.data().size();i++) {
    char c=dgram.data().at(i);
    switch(istate) {
    case 0:
      if((c>=32)&&(c<177)) {
	line+=c;
	istate=1;
      }
      break;

    case 1:
      if((c>=32)&&(c<177)) {
	line+=c;
      }
      else {
	d_lines.push_back(QString::fromUtf8(line));
	line.clear();
	istate=0;
      }
    }
  }
  if(line.size()>0) {
    d_lines.push_back(QString::fromUtf8(line));
  }
}


SyHowdyMessage::SyHowdyMessage()
  : SyMessage()
{
}


QString SyHowdyMessage::dump()
{
  QString ret="";

  ret+=QDateTime::currentDateTime().toString("MMM dd hh:mm:ss")+":"+
    " From "+hostAddress().toString()+":\n";
  for(int i=0;i<d_lines.size();i++) {
    ret+=d_lines.at(i)+"\n";
  }
  ret+="===================================================================\n";

  return ret;
}
