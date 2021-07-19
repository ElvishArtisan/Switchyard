// statuslight.h
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

#ifndef STATUSLIGHT_H
#define STATUSLIGHT_H

#include <QEvent>
#include <QWidget>

class StatusLight : public QWidget
{
 Q_OBJECT;
 public:
  StatusLight(QWidget *parent=0);
  QSize sizeHint() const;
  bool status() const;
  void setStatus(bool state);

 protected:
  void changeEvent(QEvent *e);
  void resizeEvent(QResizeEvent *e);
  void paintEvent(QPaintEvent *e);

 private:
  bool status_status;
  bool status_enabled;
};


#endif  // STATUSLIGHT_H
