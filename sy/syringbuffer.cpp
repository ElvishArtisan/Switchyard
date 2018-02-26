// syringbuffer.cpp
//
// A ringbuffer class for PCM audio
//
// (C) Copyright 2011 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdlib.h>
#include <stdio.h>

#include <math.h>
#include <string.h>

#include "syringbuffer.h"

SyRingbuffer::SyRingbuffer(unsigned frames,unsigned channels)
{
  //
  // Normalize Size
  //
  bool ok=false;
  for(unsigned i=0;i<(8*sizeof(unsigned));i++) {
    unsigned size=(unsigned)1<<i;
    if(size>=frames) {
      if(size!=frames) {
	fprintf(stderr,"SyRingbuffer: size is not an even power of two\n");
      }
      frames=size;
      ok=true;
      break;
    }
  }
  if(!ok) {
    fprintf(stderr,
	    "SyRingbuffer: unable to normalize size (requested size: %u)\n",
	    frames);
    exit(256);
  }

  ring_size=frames;
  ring_mask=ring_size-1;
  ring_channels=channels;
  ring_write_ptr=0;
  ring_read_ptr=0;
  ring_buffer=new float[(frames+1)*channels];
}


SyRingbuffer::~SyRingbuffer()
{
  delete ring_buffer;
}


unsigned SyRingbuffer::size() const
{
  return ring_size;
}


unsigned SyRingbuffer::read(float *data,unsigned frames)
{
  //
  // Normalize
  //
  if(frames>readSpace()) {
    frames=readSpace();
  }
  unsigned ret=frames;

  //
  // Stage One
  //
  unsigned start=0;
  unsigned end=0;
  if(ring_read_ptr>ring_write_ptr) {
    end=ring_read_ptr+frames;
    if(end>ring_size) {
      end=ring_size;
    }
    start=ring_read_ptr;
    CopyFrames(data,ring_buffer+(ring_channels*start),end-start);
    frames-=(end-start);
    ring_read_ptr=(ring_read_ptr+end-start)&ring_mask;
  }

  //
  // Stage Two
  //
  if(frames>0) {
    CopyFrames(data+(end-start)*ring_channels,
	       ring_buffer+ring_channels*ring_read_ptr,frames);
    ring_read_ptr=(ring_read_ptr+frames)&ring_mask;
  }

  return ret;
}


unsigned SyRingbuffer::readSpace() const
{
  if(ring_write_ptr>=ring_read_ptr) {
    return ring_write_ptr-ring_read_ptr;
  }
  return ring_size-ring_read_ptr+ring_write_ptr;
}


unsigned SyRingbuffer::write(float *data,unsigned frames)
{
  //
  // Normalize
  //
  if(frames>writeSpace()) {
    frames=writeSpace();
  }
  unsigned ret=frames;

  //
  // Stage One
  //
  unsigned start=0;
  unsigned end=0;
  if(ring_write_ptr>=ring_read_ptr) {
    end=(ring_write_ptr+frames);
    if(end>ring_size) {
      end=ring_size;
    }
    start=ring_write_ptr;
    CopyFrames(ring_buffer+ring_channels*start,data,end-start);
    ring_write_ptr=(ring_write_ptr+(end-start))&ring_mask;
    frames-=(end-start);
  }

  //
  // Stage Two
  //
  if(frames>0) {
    CopyFrames(ring_buffer+(ring_channels*ring_write_ptr),
	       data+(ring_channels*(end-start)),frames);
    ring_write_ptr=(ring_write_ptr+frames)&ring_mask;
  }

  return ret;
}


unsigned SyRingbuffer::writeSpace() const
{
  if(ring_write_ptr>=ring_read_ptr) {
    return ring_size-ring_write_ptr+ring_read_ptr-1;
  }
  return ring_read_ptr-ring_write_ptr-1;
}


void SyRingbuffer::CopyFrames(float *dst,const float *src,unsigned frames) const
{
  memcpy(dst,src,frames*ring_channels*sizeof(float));
}
