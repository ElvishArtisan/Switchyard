// sye2e_message.cpp
//
// Abstract a WheatNet E2E mMssage
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

#include "sye2e_message.h"

SyE2eMessage::SyE2eMessage(const QNetworkDatagram &dgram)
  : SyMessage(dgram)
{
  d_op_code=(SyE2eMessage::OpCode)(0xFF&dgram.data().at(3));
}


SyE2eMessage::SyE2eMessage()
  : SyMessage()
{
}


SyE2eMessage::OpCode SyE2eMessage::opCode() const
{
  return d_op_code;
}


QString SyE2eMessage::dump()
{
  QString ret="";

  ret+=hostAddress().toString()+": "+
    QString::asprintf("len: %d  op: %s\n",body().size(),SyE2eMessage::opCodeString(d_op_code).toUtf8().constData());
  ret+=toAscii(body());
  ret+=
  "=========================================================================\n";
  return ret;
}


QString SyE2eMessage::opCodeString(SyE2eMessage::OpCode code)
{
  QString ret=QString::asprintf("Unknown [0x%02X]",0xFF&code);

  switch(code) {
  case SyE2eMessage::OpCode00:
    ret=QString("OpCode00");
    break;

  case SyE2eMessage::OpCode0C:
    ret=QString("OpCode0C");
    break;

  case SyE2eMessage::OpCode0D:
    ret=QString("OpCode0D");
    break;

  case SyE2eMessage::OpCode4D:
    ret=QString("OpCode4D");
    break;

  case SyE2eMessage::OpCode53:
    ret=QString("OpCode53");
    break;

  case SyE2eMessage::OpCode54:
    ret=QString("OpCode54");
    break;

  case SyE2eMessage::OpCode5C:
    ret=QString("OpCode5C");
    break;

  case SyE2eMessage::OpCode5D:
    ret=QString("OpCode5D");
    break;

  case SyE2eMessage::OpCode5E:
    ret=QString("OpCode5E");
    break;

  case SyE2eMessage::OpCode62:
    ret=QString("OpCode62");
    break;

  case SyE2eMessage::OpCode6A:
    ret=QString("OpCode6A");
    break;

  case SyE2eMessage::OpCode6D:
    ret=QString("OpCode6D");
    break;

  case SyE2eMessage::OpCodeA1:
    ret=QString("OpCodeA1");
    break;

  case SyE2eMessage::OpCodeB6:
    ret=QString("OpCodeB6");
    break;

  case SyE2eMessage::OpCodeC1:
    ret=QString("OpCodeC1");
    break;

  case SyE2eMessage::OpCodeC2:
    ret=QString("OpCodeC2");
    break;

  case SyE2eMessage::OpCode47:
    ret=QString("OpCode47");
    break;

  case SyE2eMessage::OpCode67:
    ret=QString("OpCode67");
    break;

  case SyE2eMessage::OpCode69:
    ret=QString("OpCode69");
    break;

  case SyE2eMessage::OpCodeC9:
    ret=QString("OpCodeC9");
    break;

  case SyE2eMessage::OpCode72:
    ret=QString("OpCode72");
    break;

  case SyE2eMessage::OpCode79:
    ret=QString("OpCode79");
    break;

  case SyE2eMessage::OpCode7A:
    ret=QString("OpCode7A");
    break;

  case SyE2eMessage::OpCode87:
    ret=QString("OpCode87");
    break;

  case SyE2eMessage::OpCode94:
    ret=QString("OpCode94");
    break;

  case SyE2eMessage::OpCode9B:
    ret=QString("OpCode9B");
    break;

  case SyE2eMessage::OpCodeA0:
    ret=QString("OpCodeA0");
    break;

  case SyE2eMessage::OpCodeA4:
    ret=QString("OpCodeA4");
    break;

  case SyE2eMessage::OpCodeA8:
    ret=QString("OpCodeA8");
    break;

  case SyE2eMessage::OpCodeAA:
    ret=QString("OpCodeAA");
    break;

  case SyE2eMessage::OpCodeAB:
    ret=QString("OpCodeAB");
    break;

  case SyE2eMessage::OpCodeAD:
    ret=QString("OpCodeAD");
    break;

  case SyE2eMessage::OpCodeAF:
    ret=QString("OpCodeAF");
    break;

  case SyE2eMessage::OpCodeB5:
    ret=QString("OpCodeB5");
    break;

  case SyE2eMessage::OpCodeC5:
    ret=QString("OpCodeC5");
    break;

  case SyE2eMessage::OpCodeC6:
    ret=QString("OpCodeC6");
    break;

  case SyE2eMessage::OpCodeC7:
    ret=QString("OpCodeC7");
    break;

  case SyE2eMessage::OpCodeCD:
    ret=QString("OpCodeCD");
    break;

  case SyE2eMessage::OpCodeCE:
    ret=QString("OpCodeCE");
    break;

  case SyE2eMessage::OpCodeD9:
    ret=QString("OpCodeD9");
    break;

  case SyE2eMessage::OpCodeDA:
    ret=QString("OpCodeDA");
    break;

  case SyE2eMessage::OpCodeDB:
    ret=QString("OpCodeDB");
    break;

  case SyE2eMessage::OpCodeDC:
    ret=QString("OpCodeDC");
    break;

  case SyE2eMessage::OpCodeF9:
    ret=QString("OpCodeF9");
    break;

  case SyE2eMessage::OpCodeFD:
    ret=QString("OpCodeFD");
    break;

  case SyE2eMessage::OpCodeFC:
    ret=QString("OpCodeFC");
    break;
    /*
  case SyE2eMessage::OpCode:
    ret=QString("OpCode");
    break;
    */

  }
  
  return ret;
}


SyE2eMessage::OpCode opCodeFromString(const QString &str)
{
  for(int i=0;i<256;i++) {
    SyE2eMessage::OpCode op=(SyE2eMessage::OpCode)i;
    if(SyE2eMessage::opCodeString(op)!="OpCode00") {
      return op;
    }
  }
  
  return SyE2eMessage::OpCode00;
}
