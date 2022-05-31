// syadv_reader.cpp
//
// Reader for AoIP Stream List
//
// (C) Copyright 2014-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <QObject>
#include <QSettings>

#include "syadv_reader.h"
#include "syprofile.h"

SyAdvReader::SyAdvReader()
{
  load();
}


unsigned SyAdvReader::srcQuantity() const
{
  return adv_slots.size();
}


unsigned SyAdvReader::slot(unsigned n) const
{
  return adv_slots[n];
}


QHostAddress SyAdvReader::nodeAddress(unsigned n) const
{
  return adv_node_addresses[n];
}


QString SyAdvReader::nodeName(unsigned n) const
{
  return adv_node_names[n];
}


QHostAddress SyAdvReader::streamAddress(unsigned n) const
{
  return adv_stream_addresses[n];
}


QString SyAdvReader::sourceName(unsigned n) const
{
  return adv_source_names[n];
}


QString SyAdvReader::fullName(unsigned n) const
{
  QString ret=sourceName(n);
  if(!nodeName(n).isEmpty()) {
    ret+="@"+nodeName(n);
  }
  return ret;
}


bool SyAdvReader::load()
{
  clear();
  adv_slots.push_back(0);
  adv_node_addresses.push_back(QHostAddress());
  adv_node_names.push_back("");
  adv_stream_addresses.push_back(QHostAddress());
  adv_source_names.push_back(QObject::tr("--- OFF ---"));

#ifdef WIN32
  int count=1;
  QString key="Source 1";
  QSettings *s= new QSettings(QSettings::SystemScope,
			      SWITCHYARD_SETTINGS_ORGANIZATION,
			      SWITCHYARD_SETTINGS_APPLICATION);
  while(!s->value(key+"/Slot").isNull()) {
    adv_slots.push_back(s->value(key+"/Slot").toInt());
    adv_node_addresses.
      push_back(QHostAddress(s->value(key+"/NodeAddress").toString()));
    adv_node_names.push_back(s->value(key+"/NodeName").toString());
    adv_stream_addresses.
      push_back(QHostAddress(s->value(key+"/StreamAddress").toString()));
    adv_source_names.push_back(s->value(key+"/SourceName").toString());
    key=QString::asprintf("Source %d",count++);
  }
#else 
  int count=1;
  bool ok=false;
  QString section=QString::asprintf("Source %u",count);
  SyProfile *p=new SyProfile();
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
    section=QString::asprintf("Source %u",++count);
    slot=p->intValue(section,"Slot",0,&ok);
  }
  delete p;
#endif  // WIN32

  return true;
}


void SyAdvReader::clear()
{
  adv_slots.clear();
  adv_node_addresses.clear();
  adv_node_names.clear();
  adv_stream_addresses.clear();
  adv_source_names.clear();
}


QString SyAdvReader::dump() const
{
  QString ret;

  for(unsigned i=0;i<srcQuantity();i++) {
    ret+=QString::asprintf("Source %u:\n",i+1);
    ret+=QString::asprintf("  Node Address: %s\n",
			   nodeAddress(i).toString().toUtf8().constData());
    ret+=QString::asprintf("  Node Name: %s\n",
			   nodeName(i).toUtf8().constData());
    ret+=QString::asprintf("  Slot: %u\n",slot(i));
    ret+=QString::asprintf("  Stream Address: %s\n",
			   streamAddress(i).toString().toUtf8().constData());
    ret+=QString::asprintf("  SourceName: %s\n",
			   sourceName(i).toUtf8().constData());
    ret+="\n";
  }

  return ret;
}
