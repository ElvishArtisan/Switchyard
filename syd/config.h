// config.h
//
// Configuration values for Switchyard
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved
//

#ifndef CONFIG_H
#define CONFIG_H

#define SWITCHYARD_ADVERTS_ADDRESS "239.192.255.3"
#define SWITCHYARD_ADVERTS_PORT 4001
#define SWITCHYARD_CLOCK_ADDRESS "239.192.255.2"
#define SWITCHYARD_CLOCK_PORT 7000
#define SWITCHYARD_DEVICE_NAME "switchyard"
#define SWITCHYARD_HWID 0xFFFF
#define SWITCHYARD_LWRP_PORT 93
#define SWITCHYARD_PROTOCOL_VERSION "1.1"
#define SWITCHYARD_ROUTING_FILE "/var/cache/aoip/routing.db"
#define SWITCHYARD_SOURCES_FILE "/var/cache/aoip/sources.db"
#define SWITCHYARD_RTP_PORT 5004
#define SWITCHYARD_SLOTS 2
#define SWITCHYARD_SHM_KEY 0x59D2B831
#define SWITCHYARD_SAVESOURCES_INTERVAL 5000

extern bool global_exiting;


#endif  // CONFIG_H
