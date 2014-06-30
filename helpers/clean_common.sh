#!/bin/sh

# clean_common.sh
#
# Clean symlinks for AoIP common routines.
#
#  (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
#      All Rights Reserved.
#
# USAGE: link_common.sh <target-dir>

rm -f $1/adv_astring.cpp
rm -f $1/adv_astring.h
rm -f $1/adv_packet.cpp
rm -f $1/adv_packet.h
rm -f $1/adv_source.cpp
rm -f $1/adv_source.h
rm -f $1/adv_tag.cpp
rm -f $1/adv_tag.h
rm -f $1/cmdswitch.cpp
rm -f $1/cmdswitch.h
rm -f $1/config.h
rm -f $1/lwrp_clientconnection.cpp
rm -f $1/lwrp_clientconnection.h
rm -f $1/lwrp_server.cpp
rm -f $1/lwrp_server.h
rm -f $1/profile.cpp
rm -f $1/profile.h
rm -f $1/routing.cpp
rm -f $1/routing.h
