// syhowdy_server.h
//
// WheatNet HOWDY Logging Server
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

#ifndef SYHOWDY_SERVER_H
#define SYHOWDY_SERVER_H

#include <sy5/syhowdy_message.h>
#include <sy5/symcastsocket.h>

class SyHowdyServer : public QObject
{
  Q_OBJECT;
 public:
  SyHowdyServer(QObject *parent=0);
  ~SyHowdyServer();
  bool initialize(QString *err_msg);

 signals:
  void messageReceived(SyHowdyMessage *msg);

 private slots:
  void readyReadData();

 private:
  SyMcastSocket *d_server_socket;
};


#endif  // SYHOWDY_SERVER_H
