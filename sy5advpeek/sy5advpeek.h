// sy5advpeek.h
//
// Utility for monitoring Livewire advertising packets
//
// (C) 2026 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SY5ADVPEEK_H
#define SY5ADVPEEK_H

#include <stdint.h>

#include <QObject>

#include <sy5/syadv_server.h>
#include <sy5/syrouting.h>

#define SY5ADVPEEK_USAGE "[--advertisements[=<src-addr>][,<src-addr>][,..]] [--reservations[=<src-addr>][,<src-addr>][,..]]\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject();

  private slots:
    void advertismentReceivedData(const QHostAddress &addr,
				  const SyAdvPacket &advert);
    void reservationReceivedData(const QHostAddress &addr,
				 const SyAdvPacket &reserv);

 private:
  QString MakeTimestamp(const QString &type,const QHostAddress &addr) const;
  QString MakeFooter() const;
  bool ContainsV4Address(const QList<QHostAddress> &addrs,
			 const QHostAddress &addr) const;
  SyAdvServer *d_server;
  SyRouting *d_routing;
  bool d_advertisments;
  QList<QHostAddress> d_advert_addrs;
  bool d_reservations;
  QList<QHostAddress> d_reserve_addrs;
};


#endif  // SY5ADVPEEK_H
