// sye2e_server.h
//
// WheatNet E2E Logging Server
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

#ifndef SYE2E_SERVER_H
#define SYE2E_SERVER_H

#include <sy5/sye2e_message.h>
#include <sy5/symcastsocket.h>

class SyE2eServer : public QObject
{
  Q_OBJECT;
 public:
  SyE2eServer(QObject *parent=0);
  ~SyE2eServer();
  bool initialize(QString *err_msg);

 signals:
  void messageReceived(SyE2eMessage *msg);

 private slots:
  void readyReadData();

 private:
  SyMcastSocket *d_server_socket;
};


#endif  // SYE2E_SERVER_H
