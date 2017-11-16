// syprofile.h
//
// Class for reading INI configuration files.
//
// (C) Copyright 2013,2016 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#ifndef SYPROFILE_H
#define SYPROFILE_H

#include <vector>

#include <QHostAddress>
#include <QString>
#include <QStringList>
#include <QTime>

class SyProfileLine
{
 public:
  SyProfileLine();
  QString tag() const;
  void setTag(QString tag);
  QString value() const;
  void setValue(QString value);
  bool used() const;
  void setUsed(bool state);
  void clear();

 private:
  QString line_tag;
  QString line_value;
  bool line_used;
};


class SyProfileSection
{
 public:
  SyProfileSection();
  QString name() const;
  void setName(QString name);
  bool getValue(QString tag,QString *value) const;
  void setValueUsed(QString tag,bool state);
  void addValue(QString tag,QString value);
  void clear();
  QStringList unusedLines() const;

 private:
  QString section_name;
  std::vector<SyProfileLine> section_line;
};


class SyProfile
{
 public:
  SyProfile();
  QStringList sectionNames() const;
  QString source() const;
  bool setSource(const QString &filename);
  bool setSource(std::vector<QString> *values);
  QString stringValue(const QString &section,const QString &tag,
		      const QString &default_value="",bool *ok=0);
  int intValue(const QString &section,const QString &tag,
	       int default_value=0,bool *ok=0);
  int hexValue(const QString &section,const QString &tag,
	       int default_value=0,bool *ok=0);
  float floatValue(const QString &section,const QString &tag,
		   float default_value=0.0,bool *ok=0);
  double doubleValue(const QString &section,const QString &tag,
		    double default_value=0.0,bool *ok=0);
  bool boolValue(const QString &section,const QString &tag,
		 bool default_value=false,bool *ok=0);
  QTime timeValue(const QString &section,const QString &tag,
		  const QTime &default_value=QTime(),bool *ok=0);
  QHostAddress addressValue(const QString &section,const QString &tag,
			    const QHostAddress &default_value=QHostAddress(),
			    bool *ok=0);
  QHostAddress addressValue(const QString &section,const QString &tag,
			    const QString &default_value="",bool *ok=0);
  void clear();
  QStringList unusedLines() const;

 private:
  QString profile_source;
  std::vector<SyProfileSection> profile_section;
};


#endif  // SYPROFILE_H
