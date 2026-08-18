// symeter_server.h
//
// WheatNet METER Logging Server
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

#ifndef SYMETER_SERVER_H
#define SYMETER_SERVER_H

#include <sy5/symeter_message.h>
#include <sy5/symcastsocket.h>

class SyMeterServer : public QObject
{
  Q_OBJECT;
 public:
  SyMeterServer(QObject *parent=0);
  ~SyMeterServer();
  bool initialize(QString *err_msg);

 signals:
  void messageReceived(SyMeterMessage *msg);

 private slots:
  void readyReadData();

 private:
  SyMcastSocket *d_server_socket;
};


#endif  // SYMETER_SERVER_H
