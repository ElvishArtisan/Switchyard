// synodetest.cpp
//
// Enumerate node resources via LWRP.
//
// (C) 2014-2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <stdio.h>

#include <QtGui/QApplication>  // So we get qApp with Qt<=4.2
#include <QCoreApplication>

#include <sy/sycmdswitch.h>
#include <sy/symcastsocket.h>
#include <sy/syrouting.h>

#include "synodetest.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  QString node;

  //
  // Process Command Line
  //
  SyCmdSwitch *cmd=
    new SyCmdSwitch(qApp->argc(),qApp->argv(),"synode",VERSION,SYNODE_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--node") {
      node=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"synodetest: unrecognized option\n");
      exit(256);
    }
  }
  if(node.isEmpty()) {
    fprintf(stderr,"synodetest: you must specify a node=<hostname>\n");
    exit(256);
  }

  node_node=new SyLwrpClient(0,this);
  node_node->setTimeoutInterval(10000);
  connect(node_node,SIGNAL(connected(unsigned,bool)),
	  this,SLOT(connectedData(unsigned,bool)));
  connect(node_node,
	  SIGNAL(sourceChanged(unsigned,int,const SyNode &,const SySource &)),
	  this,
	  SLOT(sourceChangedData(unsigned,int,const SyNode &,
				 const SySource &)));
  connect(node_node,
	  SIGNAL(destinationChanged(unsigned,int,const SyNode &,
				    const SyDestination &)),
	  this,SLOT(destinationChangedData(unsigned,int,const SyNode &,
					   const SyDestination &)));
  connect(node_node,SIGNAL(gpiChanged(unsigned,int,const SyNode &,
				      const SyGpioBundle &)),
	  this,SLOT(gpiChangedData(unsigned,int,const SyNode &,
				   const SyGpioBundle &)));
  connect(node_node,SIGNAL(gpoChanged(unsigned,int,const SyNode &,
				      const SyGpo &)),
	  this,SLOT(gpoChangedData(unsigned,int,const SyNode &,const SyGpo &)));
  connect(node_node,SIGNAL(nicAddressChanged(unsigned,const QHostAddress &)),
	  this,SLOT(nicAddressChangedData(unsigned,const QHostAddress &)));
  connect(node_node,
	  SIGNAL(connectionError(unsigned,QAbstractSocket::SocketError)),
	  this,
	  SLOT(connectionErrorData(unsigned,QAbstractSocket::SocketError)));

  node_node->connectToHost(QHostAddress(node),93,"",true);
}


void MainObject::connectedData(unsigned id,bool state)
{
  fprintf(stderr,"SIGNAL: connected(%d,%u)\n",id,state);
  if(state) {
    printf("CONNECTED to %s:%u\n",
	   (const char *)node_node->hostAddress().toString().toUtf8(),
	   0xFFFF&node_node->port());
    printf("\n");
    printf("Hostname: %s\n",(const char *)node_node->hostName().toUtf8());
    printf("Host Address: %s\n",
	   (const char *)node_node->hostAddress().toString().toUtf8());
    printf("Source Slots: %u\n",node_node->srcSlots());
    printf("Destination Slots: %u\n",node_node->dstSlots());
    printf("GPI Slots: %u\n",node_node->gpis());
    printf("GPO Slots: %u\n",node_node->gpos());
    printf("\n");
    printf("--- SOURCES ---\n");
    for(unsigned i=0;i<node_node->srcSlots();i++) {
      printf("Slot %3u:\n",i+1);
      printf("  Stream Address: %s\n",
	     (const char *)node_node->srcAddress(i).toString().toUtf8());
      printf("  Source Number: %d\n",node_node->srcNumber(i));
      printf("  Name: %s\n",(const char *)node_node->srcName(i).toUtf8());
      printf("  Channels: %u\n",node_node->srcChannels(i));
      printf("  Packet Size: %u\n",node_node->srcPacketSize(i));
      if(node_node->srcEnabled(i)) {
	printf("  Enabled: Yes\n");
      }
      else {
	printf("  Enabled: No\n");
      }
      if(node_node->srcShareable(i)) {
	printf("  Shareable: Yes\n");
      }
      else {
	printf("  Shareable: No\n");
      }
      printf("\n");
    }

    printf("--- DESTINATIONS ---\n");
    for(unsigned i=0;i<node_node->dstSlots();i++) {
      printf("Slot %3u:\n",i+1);
      printf("  Stream Address: %s\n",
	     (const char *)node_node->dstAddress(i).toString().toUtf8());
      printf("  Name: %s\n",(const char *)node_node->dstName(i).toUtf8());
      printf("  Channels: %u\n",node_node->dstChannels(i));
      printf("\n");
    }

    printf("--- GPIs ---\n");
    for(unsigned i=0;i<node_node->gpis();i++) {
      printf("GPI %3u:\n",i+1);
      printf("  Code: %s\n",
	     (const char *)node_node->gpiBundle(i)->code().toUtf8());
      printf("\n");
    }  

    printf("--- GPOs ---\n");
    for(unsigned i=0;i<node_node->gpos();i++) {
      printf("GPO %3u:\n",i+1);
      printf("  Name: %s\n",(const char *)node_node->gpo(i)->name().toUtf8());
      if(node_node->gpo(i)->sourceSlot()<0) {
	printf("  Source Address: %s\n",
	       (const char *)node_node->gpo(i)->sourceAddress().toString().toUtf8());
      }
      else {
	printf("  Source Address: %s/%d\n",
	       (const char *)node_node->gpo(i)->sourceAddress().toString().toUtf8(),
	       node_node->gpo(i)->sourceSlot()+1);
      }
      printf("  Code: %s\n",
	     (const char *)node_node->gpo(i)->bundle()->code().toUtf8());
      printf("\n");
    }  
  }
}


void MainObject::connectionErrorData(unsigned id,QAbstractSocket::SocketError err)
{
  fprintf(stderr,"SIGNAL: connectionError(%u,%u) [%s]\n",id,err,
	  (const char *)SyMcastSocket::socketErrorText(err).toUtf8());
}


void MainObject::sourceChangedData(unsigned id,int slotnum,const SyNode &node,
				   const SySource &src)
{
  printf("*** SOURCE UPDATE ***\n");
  printf("Slot %3u:\n",slotnum+1);
  printf("  Stream Address: %s [%u]\n",
	 (const char *)src.streamAddress().toString().toUtf8(),
	 SyRouting::livewireNumber(src.streamAddress()));
  printf("  Name: %s\n",(const char *)src.name().toUtf8());
  printf("  Channels: %u\n",src.channels());
  printf("  Packet Size: %u\n",src.packetSize());
  if(src.enabled()) {
    printf("  Enabled: Yes\n");
  }
  else {
    printf("  Enabled: No\n");
  }
  if(src.shareable()) {
    printf("  Shareable: Yes\n");
  }
  else {
    printf("  Shareable: No\n");
  }
  printf("\n");
}


void MainObject::destinationChangedData(unsigned id,int slotnum,
					const SyNode &node,
					const SyDestination &dst)
{
  printf("*** DESTINATION UPDATE ***\n");
  printf("Slot %3u:\n",slotnum+1);
  printf("  Stream Address: %s [%u]\n",
	 (const char *)dst.streamAddress().toString().toUtf8(),
	 SyRouting::livewireNumber(dst.streamAddress()));
  printf("  Name: %s\n",(const char *)dst.name().toUtf8());
  printf("  Channels: %u\n",dst.channels());
  printf("\n");
}


void MainObject::gpiChangedData(unsigned id,int slotnum,const SyNode &node,
				const SyGpioBundle &bundle)
{
  printf("*** GPI UPDATE ***\n");
  printf("Slot %3u:\n",slotnum+1);
  printf("  Code: %s\n",(const char *)bundle.code().toUtf8());
  printf("\n");
}


void MainObject::gpoChangedData(unsigned id,int slotnum,const SyNode &node,
				const SyGpo &gpo)
{
  printf("*** GPO UPDATE ***\n");
  printf("Slot %3u:\n",slotnum+1);
  printf("  Name: %s\n",(const char *)gpo.name().toUtf8());
  if(gpo.sourceSlot()<0) {
    printf("  Source Address: %s\n",
	   (const char *)gpo.sourceAddress().toString().toUtf8());
  }
  else {
    printf("  Source Address: %s/%d\n",
	   (const char *)gpo.sourceAddress().toString().toUtf8(),
	   gpo.sourceSlot()+1);
  }
  printf("  Code: %s\n",(const char *)gpo.bundle()->code().toUtf8());
  printf("\n");
}


void MainObject::nicAddressChangedData(unsigned id,const QHostAddress &nicaddr)
{
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
