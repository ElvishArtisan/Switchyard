// playmeter.h
//
//   A playback audio meter widget.
//
//   (C) Copyright 2002-2014 Fred Gleason <fredg@paravelsystems.com>
//       All Rights Reserved
//

#ifndef PLAYMETER_H
#define PLAYMETER_H

#include <QtGui/QtGui>

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
