// sygpo.cpp
//
// Abstract a LiveWire GPO slot
//
// (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#include "sygpo.h"

SyGpo::SyGpo()
{
  gpo_bundle=new SyGpioBundle();
}


SyGpo::~SyGpo()
{
  delete gpo_bundle;
}


QString SyGpo::name() const
{
  return gpo_name;
}


void SyGpo::setName(const QString &str)
{
  gpo_name=str;
}


QHostAddress SyGpo::sourceAddress() const
{
  return gpo_source_address;
}


int SyGpo::sourceSlot() const
{
  return gpo_source_slot;
}


void SyGpo::setSourceAddress(const QHostAddress &s_addr,int s_slot)
{
  gpo_source_address=s_addr;
  gpo_source_slot=s_slot;
}


bool SyGpo::follow() const
{
  return gpo_follow;
}


void SyGpo::setFollow(bool state)
{
  gpo_follow=state;
}


SyGpioBundle *SyGpo::bundle() const
{
  return gpo_bundle;
}
