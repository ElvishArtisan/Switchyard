// syastringtest.cpp
//
// Test the SyAString parser
//
// (C) 2017 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
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
