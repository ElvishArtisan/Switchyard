// synodetest.h
//
// Enumerate node resources via LWRP.
//
// (C) 2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
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
  void connectedData(unsigned id);
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
