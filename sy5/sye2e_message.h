// sye2e_message.h
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

#ifndef SYE2E_MESSAGE_H
#define SYE2E_MESSAGE_H

#include <QByteArray>
#include <QHostAddress>
#include <QNetworkDatagram>

#include <sy5/symessage.h>

class SyE2eMessage : public SyMessage
{
 public:
  enum OpCode {OpCode00=0x00,OpCode4D=0x4d,OpCode53=0x53,OpCode54=0x54,OpCode5D=0x5d,
    OpCode62=0x62,OpCode6D=0x6d,OpCode79=0x79,OpCode7A=0x7a,OpCode94=0x94,OpCode9B=0x9b,OpCodeC1=0xC1,OpCodeC2=0xc2,

    OpCode0C=0x0c,OpCode0D=0x0d,OpCode47=0x47,OpCode5C=0x5c,OpCode5E=0x5e,OpCode67=0x67,OpCode69=0x69,OpCode6A=0x6a,OpCodeC9=0xc9,OpCode72=0x72,OpCode87=0x87,OpCodeA0=0xA0,OpCodeA1=0xa1,OpCodeA4=0xa4,OpCodeA8=0xa8,OpCodeAA=0xaa,OpCodeAB=0xab,OpCodeAD=0xad,OpCodeAF=0xaf,OpCodeB5=0xb5,OpCodeB6=0xb6,OpCodeC5=0xC5,OpCodeC6=0xc6,OpCodeC7=0xC7,OpCodeCD=0xcd,OpCodeCE=0xce,OpCodeDA=0xda,OpCodeDB=0xdb,OpCodeDC=0xdc,OpCodeD9=0xd9,
    OpCodeF9=0xf9,OpCodeFC=0xFC,OpCodeFD=0xfd
  };
  SyE2eMessage(const QNetworkDatagram &dgram);
  SyE2eMessage();
  virtual ~SyE2eMessage()=default;
  OpCode opCode() const;
  QString dump();
  static QString opCodeString(OpCode code);
  static OpCode opCodeFromString(const QString &str);

 private:
  SyE2eMessage::OpCode  d_op_code;
};


#endif  // SYE2E_MESSAGE_H
