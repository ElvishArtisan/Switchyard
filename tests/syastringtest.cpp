// syastringtest.cpp
//
// Test the SyAString parser
//
// (C) 2017 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>
#include <stdlib.h>

#include <QCoreApplication>

#include <sy/syastring.h>

#include "syastringtest.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  if(qApp->argc()<2) {
    fprintf(stderr,"syastringtest: you must specify a test string\n");
    exit(1);
  }
  printf("Test String: |%s|\n",qApp->argv()[1]);
  SyAString astr(QString(qApp->argv()[1]));
  QStringList f0=astr.split(" ","\"");
  for(int i=0;i<f0.size();i++) {
    printf("[%d]: [%s]\n",i,(const char *)f0.at(i).toUtf8());
  }
  exit(0);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);

  new MainObject();
  return a.exec();
}
