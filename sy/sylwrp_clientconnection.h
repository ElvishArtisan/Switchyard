// sylwrp_clientconnection.h
//
// Container class for LWRP client connections.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYLWRP_CLIENTCONNECTION_H
#define SYLWRP_CLIENTCONNECTION_H

#include <QMap>
#include <QString>
#include <QTcpSocket>

class SyLwrpClientConnection
{
 public:
  SyLwrpClientConnection();
  ~SyLwrpClientConnection();
  QTcpSocket *socket() const;
  void setSocket(QTcpSocket *socket);
  QString commandBuffer() const;
  void appendCommandBuffer(const char c);
  void clearBuffer();
  bool gpiAdded(int slot) const;
  void gpiAdd(int slot);
  void gpiDel(int slot);
  bool gpoAdded(int slot) const;
  void gpoAdd(int slot);
  void gpoDel(int slot);

 private:
  QTcpSocket *client_socket;
  QString client_command_buffer;
  QMap<int,bool> client_gpi_added;
  QMap<int,bool> client_gpo_added;
};


#endif  // SYLWRP_CLIENTCONNECTION_H
