// adv_reader.cpp
//
// Reader for AoIP Stream List
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <QtCore/QObject>

#include "profile.h"

#include "adv_reader.h"

AdvReader::AdvReader()
{
  load();
}


unsigned AdvReader::srcQuantity() const
{
  return adv_slots.size();
}


unsigned AdvReader::slot(unsigned n) const
{
  return adv_slots[n];
}


QHostAddress AdvReader::nodeAddress(unsigned n) const
{
  return adv_node_addresses[n];
}


QString AdvReader::nodeName(unsigned n) const
{
  return adv_node_names[n];
}


QHostAddress AdvReader::streamAddress(unsigned n) const
{
  return adv_stream_addresses[n];
}


QString AdvReader::sourceName(unsigned n) const
{
  return adv_source_names[n];
}


QString AdvReader::fullName(unsigned n) const
{
  QString ret=sourceName(n);
  if(!nodeName(n).isEmpty()) {
    ret+="@"+nodeName(n);
  }
  return ret;
}


bool AdvReader::load()
{
  clear();
  adv_slots.push_back(0);
  adv_node_addresses.push_back(QHostAddress());
  adv_node_names.push_back("");
  adv_stream_addresses.push_back(QHostAddress());
  adv_source_names.push_back(QObject::tr("--- OFF ---"));
  
  int count=1;
  bool ok=false;
  QString section=QString().sprintf("Source %u",count);
  Profile *p=new Profile();
  if(!p->setSource(SWITCHYARD_SOURCES_FILE)) {
    return false;
  }
  int slot=p->intValue(section,"Slot",0,&ok);
  while(ok) {
    adv_slots.push_back(slot);
    adv_node_addresses.push_back(p->addressValue(section,"NodeAddress",""));
    adv_node_names.push_back(p->stringValue(section,"NodeName"));
    adv_stream_addresses.push_back(p->addressValue(section,"StreamAddress",""));
    adv_source_names.push_back(p->stringValue(section,"SourceName"));
    section=QString().sprintf("Source %u",++count);
    slot=p->intValue(section,"Slot",0,&ok);
  }
  delete p;
  return true;
}


void AdvReader::clear()
{
  adv_slots.clear();
  adv_node_addresses.clear();
  adv_node_names.clear();
  adv_stream_addresses.clear();
  adv_source_names.clear();
}


QString AdvReader::dump() const
{
  QString ret;

  for(unsigned i=0;i<srcQuantity();i++) {
    ret+=QString().sprintf("Source %u:\n",i+1);
    ret+=QString().sprintf("  Node Address: %s\n",(const char *)nodeAddress(i).
	   toString().toAscii());
    ret+=QString().sprintf("  Node Name: %s\n",
			   (const char *)nodeName(i).toAscii());
    ret+=QString().sprintf("  Slot: %u\n",slot(i));
    ret+=QString().sprintf("  Stream Address: %s\n",
			   (const char *)streamAddress(i).toString().toAscii());
    ret+=QString().sprintf("  SourceName: %s\n",
			   (const char *)sourceName(i).toAscii());
    ret+="\n";
  }

  return ret;
}
