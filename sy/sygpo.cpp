// sygpo.cpp
//
// Abstract a LiveWire GPO slot
//
// (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
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
