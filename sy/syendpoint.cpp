// syendpoint.cpp
//
// Container class for LiveWire audio endpoint attributes
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

#include "syendpoint.h"

SyEndPoint::SyEndPoint()
{
  ep_clip_threshold=0;
  ep_clip_timeout=0;
  ep_silence_threshold=0;
  ep_silence_timeout=0;
  ep_exists=false;
}


int SyEndPoint::clipThreshold() const
{
  return ep_clip_threshold;
}


void SyEndPoint::setClipThreshold(int lvl)
{
  ep_clip_threshold=lvl;
}


int SyEndPoint::clipTimeout() const
{
  return ep_clip_timeout;
}


void SyEndPoint::setClipTimeout(int msec)
{
  ep_clip_timeout=msec;
}


int SyEndPoint::silenceThreshold() const
{
  return ep_silence_threshold;
}


void SyEndPoint::setSilenceThreshold(int lvl)
{
  ep_silence_threshold=lvl;
}


int SyEndPoint::silenceTimeout() const
{
  return ep_silence_timeout;
}


void SyEndPoint::setSilenceTimeout(int msec)
{
  ep_silence_timeout=msec;
}


bool SyEndPoint::exists() const
{
  return ep_exists;
}


void SyEndPoint::setExists(bool state)
{
  ep_exists=state;
}
