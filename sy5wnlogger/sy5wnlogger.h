// sy5wnlogger.h
//
// Print WheatNet LO log messages
//
// (C) 2026 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SY5WNLOGGER_H
#define SY5WNLOGGER_H

#include <sy5/sye2e_server.h>
#include <sy5/syhowdy_server.h>
#include <sy5/sylo_server.h>
#include <sy5/symeter_server.h>

#define SY5WNLOGGER_USAGE "--proto=lo|e2e|howdy|meter [--dump-body] [--host-address=<ipv4-addr ...]\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  enum Proto {ProtoLo=0,ProtoE2e=1,ProtoMeter=2,ProtoHowdy=3,ProtoLast=4};
  MainObject(QObject *parent=0);

 private slots:
  void messageReceivedData(SyE2eMessage *msg);
  void messageReceivedData(SyHowdyMessage *msg);
  void messageReceivedData(SyLoMessage *msg);
  void messageReceivedData(SyMeterMessage *msg);

 private:
  SyE2eServer *d_e2e_server;
  SyHowdyServer *d_howdy_server;
  SyLoServer *d_lo_server;
  SyMeterServer *d_meter_server;
  Proto d_proto;
  bool d_dump_body;
  QStringList d_opcodes_seen;
  QList<QHostAddress> d_host_addresses;
};


#endif  // SY5WNLOGGER_H
