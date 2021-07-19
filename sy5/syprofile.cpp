// syprofile.cpp
//
// A container class for profile lines.
//
// (C) Copyright 2013,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QFile>
#include <QStringList>
#include <QTextStream>

#include "syprofile.h"

SyProfileLine::SyProfileLine()
{
  clear();
}


QString SyProfileLine::tag() const
{
  return line_tag;
}


void SyProfileLine::setTag(QString tag)
{
  line_tag=tag;
}


QString SyProfileLine::value() const
{
  return line_value;
}


void SyProfileLine::setValue(QString value)
{
  line_value=value;
}


bool SyProfileLine::used() const
{
  return line_used;
}


void SyProfileLine::setUsed(bool state)
{
  line_used=state;
}


void SyProfileLine::clear()
{
  line_tag="";
  line_value="";
  line_used=false;
}


SyProfileSection::SyProfileSection()
{
  clear();
}


QString SyProfileSection::name() const
{
  return section_name;
}


void SyProfileSection::setName(QString name)
{
  section_name=name;
}


bool SyProfileSection::getValue(QString tag,QString *value) const
{
  for(unsigned i=0;i<section_line.size();i++) {
    if(section_line[i].tag()==tag) {
      *value=section_line[i].value();
      return true;
    }
  }
  return false;
}


void SyProfileSection::setValueUsed(QString tag,bool state)
{
  for(unsigned i=0;i<section_line.size();i++) {
    if(section_line[i].tag()==tag) {
      section_line[i].setUsed(state);
      return;
    }
  }  
}


void SyProfileSection::addValue(QString tag,QString value)
{
  section_line.push_back(SyProfileLine());
  section_line.back().setTag(tag);
  section_line.back().setValue(value);
}


void SyProfileSection::clear()
{
  section_name="";
  section_line.resize(0);
}


QStringList SyProfileSection::unusedLines() const
{
  QStringList ret;

  for(unsigned i=0;i<section_line.size();i++) {
    if(!section_line.at(i).used()) {
      ret.push_back(section_line.at(i).tag()+"="+section_line.at(i).value());
    }
  }

  return ret;
}


SyProfile::SyProfile()
{
}


QStringList SyProfile::sectionNames() const
{
  QStringList names;

  for(unsigned i=0;i<profile_section.size();i++) {
    if(!profile_section.at(i).name().isEmpty()) {
      names.push_back(profile_section.at(i).name());
    }
  }

  return names;
}


QString SyProfile::source() const
{
  return profile_source;
}


bool SyProfile::setSource(const QString &filename)
{
  QString section;
  int offset;

  profile_source=filename;
  profile_section.resize(0);
  profile_section.push_back(SyProfileSection());
  profile_section.back().setName("");
  QFile *file=new QFile(filename);
  if(!file->open(QIODevice::ReadOnly)) {
    delete file;
    return false;
  }
  QTextStream *text=new QTextStream(file);
  QString line=text->readLine().trimmed();
  while(!line.isNull()) {
    if((line.left(1)!=";")&&(line.left(1)!="#")) {
      if((line.left(1)=="[")&&(line.right(1)=="]")) {
	section=line.mid(1,line.length()-2);
	profile_section.push_back(SyProfileSection());
	profile_section.back().setName(section);
      }
      else if(((offset=line.indexOf('='))!=-1)) {
	profile_section.back().
	  addValue(line.left(offset),
		   line.right(line.length()-offset-1).trimmed());
      }
    }
    line=text->readLine().trimmed();
  }
  delete text;
  delete file;
  return true;
}


bool SyProfile::setSource(std::vector<QString> *values)
{
  QString section;
  int offset;

  profile_section.resize(0);
  profile_section.push_back(SyProfileSection());
  profile_section.back().setName("");
  for(unsigned i=0;i<values->size();i++) {
    if((values->at(i).left(1)!=";")&&(values->at(i).left(1)!="#")) {
      if((values->at(i).left(1)=="[")&&(values->at(i).right(1)=="]")) {
	section=values->at(i).mid(1,values->at(i).length()-2);
	profile_section.push_back(SyProfileSection());
	profile_section.back().setName(section);
      }
      else if(((offset=values->at(i).indexOf('='))!=-1)) {
	profile_section.back().
	  addValue(values->at(i).left(offset),
		   values->at(i).right(values->at(i).length()-offset-1).
		   trimmed());
      }
    }
  }
  return true;
}


QString SyProfile::stringValue(const QString &section,const QString &tag,
			       const QString &default_str,bool *ok)
{
  QString result;

  for(unsigned i=0;i<profile_section.size();i++) {
    if(profile_section[i].name()==section) {
      if(profile_section[i].getValue(tag,&result)) {
	if(ok!=NULL) {
	  *ok=true;
	}
	profile_section[i].setValueUsed(tag,true);
	return result;
      }
      if(ok!=NULL) {
	*ok=false;
      }
      return default_str;
    }
  }
  if(ok!=NULL) {
    *ok=false;
  }
  return default_str;
}


int SyProfile::intValue(const QString &section,const QString &tag,
		       int default_value,bool *ok)
{
  bool valid;

  int result=stringValue(section,tag).toInt(&valid,10);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


int SyProfile::hexValue(const QString &section,const QString &tag,
		       int default_value,bool *ok)
{
  bool valid;

  int result=stringValue(section,tag).toInt(&valid,16);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


float SyProfile::floatValue(const QString &section,const QString &tag,
			   float default_value,bool *ok)
{
  bool valid;

  float result=stringValue(section,tag).toDouble(&valid);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


double SyProfile::doubleValue(const QString &section,const QString &tag,
			    double default_value,bool *ok)
{
  bool valid;

  double result=stringValue(section,tag).toDouble(&valid);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


bool SyProfile::boolValue(const QString &section,const QString &tag,
			 bool default_value,bool *ok)
{
  bool valid;

  QString str=stringValue(section,tag,"",&valid).toLower();
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if((str=="yes")||(str=="true")||(str=="on")) {
    if(ok!=NULL) {
      *ok=true;
    }
    return true;
  }
  if((str=="no")||(str=="false")||(str=="off")) {
    if(ok!=NULL) {
      *ok=true;
    }
    return false;
  }
  if(ok!=NULL) {
    *ok=false;
  }
  return default_value;
}


QTime SyProfile::timeValue(const QString &section,const QString &tag,
			   const QTime &default_value,bool *ok)
{
  QStringList fields;
  bool ok1=false;
  QString str=stringValue(section,tag,"",&ok1);
  QTime ret(default_value);

  if(ok1) {
    fields=str.split(":");
    if(fields.size()==2) {
      ret.setHMS(fields[0].toInt(),fields[1].toInt(),0);
    }
    if(fields.size()==3) {
      ret.setHMS(fields[0].toInt(),fields[1].toInt(),fields[2].toInt());
    }
  }
  
  return ret;
}


QHostAddress SyProfile::addressValue(const QString &section,const QString &tag,
				     const QHostAddress &default_value,bool *ok)
{
  return QHostAddress(stringValue(section,tag,default_value.toString(),ok));
}


QHostAddress SyProfile::addressValue(const QString &section,const QString &tag,
				     const QString &default_value,bool *ok)
{
  return addressValue(section,tag,QHostAddress(default_value),ok);
}


void SyProfile::clear()
{
  profile_source="";
  profile_section.resize(0);
}


QStringList SyProfile::unusedLines() const
{
  QStringList ret;

  for(unsigned i=0;i<profile_section.size();i++) {
    QStringList lines=profile_section.at(i).unusedLines();
    if(lines.size()>0) {
      ret.push_back("["+profile_section.at(i).name()+"]");
      for(int j=0;j<lines.size();j++) {
	ret.push_back(lines.at(j));
      }
    }
  }

  return ret;
}
