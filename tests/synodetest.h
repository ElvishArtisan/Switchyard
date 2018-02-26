// synodetest.h
//
// Enumerate node resources via LWRP.
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

#ifndef SYNODETEST_H
#define SYNODETEST_H

#include <QObject>

#include <sy/sylwrp_client.h>

#define SYNODE_USAGE "--node=<hostname>\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void connectedData(unsigned id,bool state);
  void connectionErrorData(unsigned id,QAbstractSocket::SocketError err);
  void sourceChangedData(unsigned id,int slotnum,const SyNode &node,
			 const SySource &src);
  void destinationChangedData(unsigned id,int slotnum,const SyNode &node,
			      const SyDestination &dst);
  void gpiChangedData(unsigned id,int slotnum,const SyNode &node,
		      const SyGpioBundle &bundle);
  void gpoChangedData(unsigned id,int slotnum,const SyNode &node,
		      const SyGpo &gpo);
  void nicAddressChangedData(unsigned id,const QHostAddress &nicaddr);

 private:
  SyLwrpClient *node_node;
};


#endif  // SYNODETEST_H
