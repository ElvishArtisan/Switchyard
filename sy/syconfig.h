// syconfig.h
//
// Configuration values for AoIP routines
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYCONFIG_H
#define SYCONFIG_H

#define SWITCHYARD_LWRP_VERSION "1.1"
#define SWITCHYARD_LWRP_CONNECTION_TIMEOUT 5000
//#define SWITCHYARD_LWRP_WATCHDOG_INTERVAL_MIN 60000.0
//#define SWITCHYARD_LWRP_WATCHDOG_INTERVAL_MAX 180000.0
#define SWITCHYARD_LWRP_WATCHDOG_INTERVAL_MIN 5000.0
#define SWITCHYARD_LWRP_WATCHDOG_INTERVAL_MAX 10000.0
#define SWITCHYARD_LWRP_WATCHDOG_RETRY 10000
#define SWITCHYARD_ADVERTS_ADDRESS "239.192.255.3"
#define SWITCHYARD_ADVERTS_PORT 4001
#define SWITCHYARD_CLOCK_ADDRESS "239.192.255.2"
#define SWITCHYARD_CLOCK_PORT 7000
#define SWITCHYARD_DEVICE_NAME "switchyard"
#define SWITCHYARD_GPIO_ADDRESS "239.192.255.4"
#define SWITCHYARD_GPIO_BUNDLE_SIZE 5
#define SWITCHYARD_GPIO_GPI_PORT 2055
#define SWITCHYARD_GPIO_GPO_PORT 2060
#define SWITCHYARD_HWID 0xFFFF
#define SWITCHYARD_LWRP_PORT 93
#define SWITCHYARD_MAX_SLOTS 32
#define SWITCHYARD_MAX_CHANNELS 2
#define SWITCHYARD_PROTOCOL_VERSION "1.1"
#define SWITCHYARD_CACHE_DIR "/var/cache/aoip"
#define SWITCHYARD_ROUTING_FILE "/var/cache/aoip/routing.db"
#define SWITCHYARD_SOURCES_FILE "/var/cache/aoip/sources.db"
#define SWITCHYARD_RTP_PORT 5004
#define SWITCHYARD_SHM_KEY 0x59D2B831
#define SWITCHYARD_SAVESOURCES_INTERVAL 5000
#define SWITCHYARD_SETTINGS_ORGANIZATION "Paravel Systems"
#define SWITCHYARD_SETTINGS_APPLICATION "Switchyard"

#endif  // SYCONFIG_H
