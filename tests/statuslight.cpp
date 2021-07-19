// statuslight.cpp
//
// Status Light Widget
//
//   (C) Copyright 2012-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QPainter>
#include <QPixmap>

#include "statuslight.h"

StatusLight::StatusLight(QWidget *parent)
  : QWidget(parent)
{
  status_status=false;
  status_enabled=true;
}


QSize StatusLight::sizeHint() const
{
  return QSize(20,20);
}


bool StatusLight::status() const
{
  return status_status;
}


void StatusLight::setStatus(bool state)
{
  if(state!=status_status) {
    status_status=state;
    update();
  }
}


void StatusLight::changeEvent(QEvent *e)
{
  switch(e->type()) {
  case QEvent::EnabledChange:
    e->accept();
    status_enabled=!status_enabled;
    update();
    break;

  default:
    QWidget::event(e);
    break;
  }
}


void StatusLight::resizeEvent(QResizeEvent *e)
{
  update();
}


void StatusLight::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);

  p->fillRect(0,0,size().width(),size().height(),
  	      palette().color(QPalette::Window));
  
  p->setPen(palette().color(QPalette::Shadow));
  p->setBrush(palette().color(QPalette::Shadow));
  p->drawPie(2,2,size().width()-4,size().height()-4,0,360*16);

  if(status_enabled) {
    if(status_status) {
      p->setPen(Qt::green);
      p->setBrush(Qt::green);
    }
    else {
      p->setPen(Qt::lightGray);
      p->setBrush(Qt::lightGray);
    }
  }
  else {
    p->setPen(palette().color(QPalette::Window));
    p->setBrush(palette().color(QPalette::Window));
  }
  p->drawPie(4,4,size().width()-8,size().height()-8,0,360*16);

  delete p;
}
