// playmeter.h
//
//   A playback audio meter widget.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef PLAYMETER_H
#define PLAYMETER_H

#include <QTimer>
#include <QWidget>

/*
 * Default Colors
 */
#define DEFAULT_LOW_COLOR Qt::green
#define DEFAULT_DARK_LOW_COLOR 0,80,0
#define DEFAULT_HIGH_COLOR Qt::yellow
#define DEFAULT_DARK_HIGH_COLOR 75,75,0
#define DEFAULT_CLIP_COLOR Qt::red
#define DEFAULT_DARK_CLIP_COLOR 85,0,0

/*
 * Global Settings
 */
#define PEAK_HOLD_TIME 750

class SegMeter : public QWidget
{
 Q_OBJECT
 public:
  enum Mode {Independent=0,Peak=1};
  enum Orientation {Left=0,Right=1,Up=2,Down=3};
  SegMeter(SegMeter::Orientation o,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void setRange(int min,int max);
  void setDarkLowColor(QColor color);
  void setDarkHighColor(QColor color);
  void setDarkClipColor(QColor color);
  void setLowColor(QColor color);
  void setHighColor(QColor color);
  void setClipColor(QColor color);
  void setHighThreshold(int level);
  void setClipThreshold(int level);
  void setSegmentSize(int size);
  void setSegmentGap(int gap);
  SegMeter::Mode mode() const;
  void setMode(SegMeter::Mode mode);

 public slots:
  void setSolidBar(int level);
  void setFloatingBar(int level);
  void setPeakBar(int level);

 protected:
  void paintEvent(QPaintEvent *);

 private slots:
  void peakData();

 private:
  SegMeter::Orientation orient;
  SegMeter::Mode seg_mode;
  QTimer *peak_timer;
  int range_min,range_max;
  QColor dark_low_color;
  QColor dark_high_color;
  QColor dark_clip_color;
  QColor low_color;
  QColor high_color;
  QColor clip_color;
  int high_threshold,clip_threshold;
  int solid_bar,floating_bar;
  int seg_size,seg_gap;
};


class PlayMeter : public QWidget
{
 Q_OBJECT
 public:
  PlayMeter(SegMeter::Orientation orient,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void setRange(int min,int max);
  void setDarkLowColor(QColor color);
  void setDarkHighColor(QColor color);
  void setDarkClipColor(QColor color);
  void setLowColor(QColor color);
  void setHighColor(QColor color);
  void setClipColor(QColor color);
  void setHighThreshold(int level);
  void setClipThreshold(int level);
  void setSegmentSize(int size);
  void setSegmentGap(int gap);
  SegMeter::Mode mode() const;
  void setMode(SegMeter::Mode mode);
  void setLabel(QString label);

 public slots:
  void setGeometry(int x,int y,int w,int h);
  void setGeometry(QRect &rect);
  void setSolidBar(int level);
  void setFloatingBar(int level);
  void setPeakBar(int level);

 protected:
  void paintEvent(QPaintEvent *);

 private:
  void makeFont();
  SegMeter *meter;
  QString meter_label;
  QFont label_font;
  SegMeter::Orientation orientation;
  int meter_label_x;
};


#endif  // PLAYMETER_H
