#!/bin/sh

# link_common.sh
#
# Create symlinks for AoIP common routines.
#
#  (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
#      All Rights Reserved.
#
# USAGE: link_common.sh <target-dir>

rm -f $1/adv_packet.cpp
ln -s ../common/adv_packet.cpp $1/adv_packet.cpp
rm -f $1/adv_packet.h
ln -s ../common/adv_packet.h $1/adv_packet.h
rm -f $1/adv_reader.cpp
ln -s ../common/adv_reader.cpp $1/adv_reader.cpp
rm -f $1/adv_reader.h
ln -s ../common/adv_reader.h $1/adv_reader.h
rm -f $1/adv_server.cpp
ln -s ../common/adv_server.cpp $1/adv_server.cpp
rm -f $1/adv_server.h
ln -s ../common/adv_server.h $1/adv_server.h
rm -f $1/adv_source.cp
ln -s ../common/adv_source.cpp $1/adv_source.cpp
rm -f $1/adv_source.
ln -s ../common/adv_source.h $1/adv_source.h
rm -f $1/adv_tag.cpp
ln -s ../common/adv_tag.cpp $1/adv_tag.cpp
rm -f $1/adv_tag.h
ln -s ../common/adv_tag.h $1/adv_tag.h
rm -f $1/astring.cpp
ln -s ../common/astring.cpp $1/astring.cpp
rm -f $1/astring.h
ln -s ../common/astring.h $1/astring.h
rm -f $1/cmdswitch.cpp
ln -s ../common/cmdswitch.cpp $1/cmdswitch.cpp
rm -f $1/cmdswitch.h
ln -s ../common/cmdswitch.h $1/cmdswitch.h
rm -f $1/config.h
ln -s ../common/config.h $1/config.h
rm -f $1/gpio_server.cpp
ln -s ../common/gpio_server.cpp $1/gpio_server.cpp
rm -f $1/gpio_server.h
ln -s ../common/gpio_server.h $1/gpio_server.h
rm -f $1/lwrp_clientconnection.cp
ln -s ../common/lwrp_clientconnection.cpp $1/lwrp_clientconnection.cpp
rm -f $1/lwrp_clientconnection.
ln -s ../common/lwrp_clientconnection.h $1/lwrp_clientconnection.h
rm -f $1/lwrp_server.cp
ln -s ../common/lwrp_server.cpp $1/lwrp_server.cpp
rm -f $1/lwrp_server.h
ln -s ../common/lwrp_server.h $1/lwrp_server.h
rm -f $1/profile.cpp
ln -s ../common/profile.cpp $1/profile.cpp
rm -f $1/profile.
ln -s ../common/profile.h $1/profile.h
rm -f $1/routing.cpp
ln -s ../common/routing.cpp $1/routing.cpp
rm -f $1/routing.h
ln -s ../common/routing.h $1/routing.h
rm -f $1/rtp.cpp
ln -s ../common/rtp.cpp $1/rtp.cpp
rm -f $1/rtp.h
ln -s ../common/rtp.h $1/rtp.h
