// lwpacket.cpp
//
// Abstract a LiveWire Control Protocol packet.
//
// (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: lwpacket.cpp,v 1.8 2010/10/21 10:44:28 pcvs Exp $
//
//   All Rights Reserved.
//

#include <stdlib.h>
#include <stdio.h>

#include <QtCore/QString>

#include <lwpacket.h>

LwPacket::LwPacket()
{
  lw_sequence_number=rand();
}


LwPacket::~LwPacket()
{
  for(unsigned i=0;i<lw_tags.size();i++) {
    delete lw_tags[i];
  }
  lw_tags.clear();
}


uint32_t LwPacket::sequenceNumber() const
{
  return lw_sequence_number;
}


void LwPacket::setSequenceNumber(uint32_t num)
{
  lw_sequence_number=num;
}


unsigned LwPacket::tags() const
{
  return lw_tags.size();
}


LwTag *LwPacket::tag(unsigned n)
{
  return lw_tags[n];
}


void LwPacket::addTag(const LwTag &tag)
{
  lw_tags.push_back(new LwTag());
  *(lw_tags.back())=tag;
}


bool LwPacket::readPacket(uint8_t *data,uint32_t size)
{
  QString tag;
  QVariant value;
  LwTag::TagType type=LwTag::TagLast;
  unsigned len=0;
  QByteArray *bytearray;
  unsigned istate=0;

  if(size<16) {
    return false;
  }
  lw_sequence_number=((0xff&data[4])<<24)+((0xff&data[5])<<16)+
    ((0xff&data[6])<<8)+(0xff&data[7]);
  /*
  for(uint32_t i=16;i<20;i++) {
    tag+=data[i];
  }
  */
  for(uint32_t i=16;i<size;i++) {
    switch(istate) {
    case 0:
      tag=data[i];
      istate=1;
      break;

    case 1:
    case 2:
    case 3:
      tag+=data[i];
      istate++;
      break;

    case 4:
      type=(LwTag::TagType)(0xff&data[i]);
      switch(type) {
      case LwTag::TagType0:  // Single Byte
      case LwTag::TagType7:
	value=QVariant((uint8_t)(0xff&data[i+1]));
	i+=1;
	istate=0;
	break;
      
      case LwTag::TagType1:  // IPv4 Address
	value.setValue((uint32_t)(((0xff&data[i+1])<<24)|((0xff&data[i+2])<<16)|
				  ((0xff&data[i+3])<<8)|(0xff&data[i+4])));
	/*
	value=QVariant((uint32_t)(((0xff&data[i+1])<<24)+((0xff&data[i+2])<<16)+
				  ((0xff&data[i+3])<<8)+(0xff&data[i+4])));
	*/
	i+=4;
	istate=0;
	break;
      
      case LwTag::TagString:
	len=((0xff&data[i+1])<<8)+(0xff&data[i+2]);
	bytearray=new QByteArray(len,'\0');
	for(unsigned j=i+3;j<(i+3+len);j++) {
	  (*bytearray)[j-i-3]=data[j];
	}
	value=QVariant(*bytearray);
	delete bytearray;
	i+=(2+len);
	istate=0;
	break;
      
      case LwTag::TagMeter:
	len=2*(((0xff&data[i+1])<<8)+(0xff&data[i+2]));
	bytearray=new QByteArray(len,'\0');
	for(unsigned j=i+3;j<(i+3+len);j++) {
	  (*bytearray)[j-i-3]=data[j];
	}
	value=QVariant(*bytearray);
	delete bytearray;
	i+=(2+len);
	istate=0;
	break;
      
      case LwTag::TagType6:  // Two Bytes
      case LwTag::TagType8:
	value.setValue((uint)(((0xff&data[i+1])<<8)|(0xff&data[i+2])));
	i+=2;
	istate=0;
	break;
      
      case LwTag::TagType9:  // Eight Bytes
	value=QVariant((qulonglong)(((uint64_t)(0xff&data[i+1])<<56)+
				    ((uint64_t)(0xff&data[i+2])<<48)+
				    ((uint64_t)(0xff&data[i+3])<<40)+
				    ((uint64_t)(0xff&data[i+4])<<32)+
				    ((uint64_t)(0xff&data[i+5])<<24)+
				    ((uint64_t)(0xff&data[i+6])<<16)+
				    ((uint64_t)(0xff&data[i+7])<<8)+
				    (uint64_t)(0xff&data[i+8])));
	i+=8;
	istate=0;
	break;
      
      default:
	len=size-i-1;
	bytearray=new QByteArray(len,'\0');
	for(unsigned j=i+2;j<(i+2+len);j++) {
	  (*bytearray)[j-i-2]=data[j];
	}
	value=QVariant(*bytearray);
	delete bytearray;
	i+=(1+len);
	fprintf(stderr,"LwPacket: unknown LWCP data type %u\n",type);
	fprintf(stderr,"packet dump: %s\n",(const char *)dump().toAscii());
	istate=0;
	break;
      }
      lw_tags.push_back(new LwTag());
      lw_tags.back()->setTagName(tag);
      lw_tags.back()->setTagValue(type,value);
      break;
    }
    /*
    tag="";
    value=QVariant();
    type=LwTag::TagLast;
    */
  }
  return true;
}


int LwPacket::writePacket(uint8_t *data,uint32_t maxsize)
{
  //
  // Generate header
  //
  if(maxsize<16) {
    return -1;
  }
  data[0]=0x03;
  data[1]=0x00;
  data[2]=0x02;
  data[3]=0x07;
  data[4]=0xff&(lw_sequence_number>>24);
  data[5]=0xff&(lw_sequence_number>>16);
  data[6]=0xff&(lw_sequence_number>>8);
  data[7]=0xff&lw_sequence_number;
  memset(data+8,0,8);
  maxsize-=16;

  //
  // Generate tags
  //
  unsigned ptr=16;
  for(unsigned i=0;i<lw_tags.size();i++) {
    ptr+=snprintf((char *)data+ptr,maxsize-ptr,"%s%c",
		  (const char *)lw_tags[i]->tagName().toAscii(),
		  lw_tags[i]->tagType());
    if(ptr>=maxsize) {
      return -1;
    }
    switch(lw_tags[i]->tagType()) {
    case LwTag::TagType0:  // Single byte
    case LwTag::TagType7:
      ptr+=snprintf((char *)data+ptr,maxsize-ptr,"%c",
		    0xff&lw_tags[i]->tagValue().toUInt());
      if(ptr>=maxsize) {
	return -1;
      }
      break;

    case LwTag::TagType1:  // Four bytes
      ptr+=snprintf((char *)data+ptr,maxsize-ptr,"%c%c%c%c",
		    0xff&(lw_tags[i]->tagValue().toUInt()>>24),
		    0xff&(lw_tags[i]->tagValue().toUInt()>>16),
		    0xff&(lw_tags[i]->tagValue().toUInt()>>8),
		    0xff&lw_tags[i]->tagValue().toUInt());
      if(ptr>=maxsize) {
	return -1;
      }
      break;

    case LwTag::TagType6:  // Two bytes
    case LwTag::TagType8:
      ptr+=snprintf((char *)data+ptr,maxsize-ptr,"%c%c",
		    0xff&(lw_tags[i]->tagValue().toUInt()>>8),
		    0xff&lw_tags[i]->tagValue().toUInt());
      if(ptr>=maxsize) {
	return -1;
      }
      break;

    case LwTag::TagType9:  // Eight bytes
      ptr+=snprintf((char *)data+ptr,maxsize-ptr,"%c%c%c%c%c%c%c%c",
		    (int)(0xff&(lw_tags[i]->tagValue().toULongLong()>>56)),
		    (int)(0xff&(lw_tags[i]->tagValue().toULongLong()>>48)),
		    (int)(0xff&(lw_tags[i]->tagValue().toULongLong()>>40)),
		    (int)(0xff&(lw_tags[i]->tagValue().toULongLong()>>32)),
		    (int)(0xff&(lw_tags[i]->tagValue().toULongLong()>>24)),
		    (int)(0xff&(lw_tags[i]->tagValue().toULongLong()>>17)),
		    (int)(0xff&(lw_tags[i]->tagValue().toULongLong()>>8)),
		    (int)lw_tags[i]->tagValue().toULongLong());
      if(ptr>=maxsize) {
	return -1;
      }
      break;

    case LwTag::TagString:
      ptr+=snprintf((char *)data+ptr,maxsize-ptr,"%c%c",
		    0xff&(lw_tags[i]->tagValue().toByteArray().size()>>8),
		    0xff&lw_tags[i]->tagValue().toByteArray().size());
      if(ptr>=maxsize) {
	return -1;
      }      
      for(int j=0;j<lw_tags[i]->tagValue().toByteArray().size();j++) {
	ptr+=snprintf((char *)data+ptr,maxsize-ptr,"%c",
		      (char)lw_tags[i]->tagValue().toByteArray()[j]);
      }
      if(ptr>=maxsize) {
	return -1;
      }
      /*
      if(lw_tags[i]->tagLength()>0) {
	memset(data+ptr,0,lw_tags[i]->tagLength());
	ptr+=lw_tags[i]->tagLength();
      }
      if(ptr>=maxsize) {
	return -1;
      }
      */
      break;

    case LwTag::TagMeter:
      ptr+=snprintf((char *)data+ptr,maxsize-ptr,"%c%c",
		    0xff&((lw_tags[i]->tagValue().toByteArray().size()/2)>>8),
		    0xff&(lw_tags[i]->tagValue().toByteArray().size()/2));
      if(ptr>=maxsize) {
	return -1;
      }      
      for(int j=0;j<lw_tags[i]->tagValue().toByteArray().size();j++) {
	ptr+=snprintf((char *)data+ptr,maxsize-ptr,"%c",
		      (char)lw_tags[i]->tagValue().toByteArray()[j]);
      }
      if(ptr>=maxsize) {
	return -1;
      }
      break;
	
    default:
      break;
    }
  }
  maxsize=ptr;
  return ptr;
}


QString LwPacket::dump() const
{
  QString str;

  str+="LWCP Packet\n";
  str+=QString().sprintf(" Seq No: %u\n",sequenceNumber());
  for(unsigned i=0;i<lw_tags.size();i++) {
    str+=LwTag::normalizeName(lw_tags[i]->tagName())+": ";
    switch(lw_tags[i]->tagType()) {
    case LwTag::TagType0:
    case LwTag::TagType7:
      str+=QString().sprintf("%02X",0xff&lw_tags[i]->tagValue().toUInt());
      break;

    case LwTag::TagType6:
    case LwTag::TagType8:
      str+=QString().sprintf("%02X %02X",
			     0xff&(lw_tags[i]->tagValue().toUInt()>>8),
			     0xff&lw_tags[i]->tagValue().toUInt());
      break;

    case LwTag::TagType1:
      str+=QString().sprintf("%02X %02X %02X %02X",
			     0xff&(lw_tags[i]->tagValue().toUInt()>>24),
			     0xff&(lw_tags[i]->tagValue().toUInt()>>16),
			     0xff&(lw_tags[i]->tagValue().toUInt()>>8),
			     0xff&lw_tags[i]->tagValue().toUInt());
      break;

    case LwTag::TagType9:
      str+=QString().
	sprintf("%02X %02X %02X %02X %02X %02X %02X %02X",
		(unsigned)(0xff&(lw_tags[i]->tagValue().toULongLong()>>56)),
		(unsigned)(0xff&(lw_tags[i]->tagValue().toULongLong()>>48)),
		(unsigned)(0xff&(lw_tags[i]->tagValue().toULongLong()>>40)),
		(unsigned)(0xff&(lw_tags[i]->tagValue().toULongLong()>>32)),
		(unsigned)(0xff&(lw_tags[i]->tagValue().toULongLong()>>24)),
		(unsigned)(0xff&(lw_tags[i]->tagValue().toULongLong()>>16)),
		(unsigned)(0xff&(lw_tags[i]->tagValue().toULongLong()>>8)),
		(unsigned)(0xff&lw_tags[i]->tagValue().toULongLong()));
      break;

    case LwTag::TagString:
      str+=QString().sprintf("\"%s\" {",
			     (const char *)lw_tags[i]->tagValue().toString().
			     toAscii());
      for(int j=0;j<lw_tags[i]->tagValue().toByteArray().size();j++) {
	str+=QString().sprintf("%02X ",
			       0xff&lw_tags[i]->tagValue().toByteArray()[j]);
      }
      str=str.left(str.length()-1);
      str+="}";
      break;

    case LwTag::TagMeter:
      str+="METER UPDATE";
      break;

    default:
      break;
    }
    str+="\n";
  }
  return str;
}


LwPacket &LwPacket::operator++()
{
  lw_sequence_number++;
  return *this;
}


LwPacket LwPacket::operator++(int)
{
  LwPacket ret=*this;
  operator++();
  return ret;
}
