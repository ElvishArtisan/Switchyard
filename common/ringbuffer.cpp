// ringbuffer.cpp
//
// A ringbuffer class for PCM audio
//
// (C) Copyright 2011 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <stdlib.h>
#include <stdio.h>

#include <math.h>
#include <string.h>

#include "ringbuffer.h"

Ringbuffer::Ringbuffer(unsigned frames,unsigned channels)
{
  //
  // Normalize Size
  //
  bool ok=false;
  for(unsigned i=0;i<(8*sizeof(unsigned));i++) {
    unsigned size=(unsigned)1<<i;
    if(size>=frames) {
      if(size!=frames) {
	fprintf(stderr,"Ringbuffer: size is not an even power of two\n");
      }
      frames=size;
      ok=true;
      break;
    }
  }
  if(!ok) {
    fprintf(stderr,
	    "Ringbuffer: unable to normalize size (requested size: %u)\n",
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


Ringbuffer::~Ringbuffer()
{
  delete ring_buffer;
}


unsigned Ringbuffer::size() const
{
  return ring_size;
}


unsigned Ringbuffer::read(float *data,unsigned frames)
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


unsigned Ringbuffer::readSpace() const
{
  if(ring_write_ptr>=ring_read_ptr) {
    return ring_write_ptr-ring_read_ptr;
  }
  return ring_size-ring_read_ptr+ring_write_ptr;
}


unsigned Ringbuffer::write(float *data,unsigned frames)
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


unsigned Ringbuffer::writeSpace() const
{
  if(ring_write_ptr>=ring_read_ptr) {
    return ring_size-ring_write_ptr+ring_read_ptr-1;
  }
  return ring_read_ptr-ring_write_ptr-1;
}


void Ringbuffer::CopyFrames(float *dst,const float *src,unsigned frames) const
{
  memcpy(dst,src,frames*ring_channels*sizeof(float));
}
