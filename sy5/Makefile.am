## Makefile.am
##
## Makefile for AoIP library routines
##
## (C) Copyright 2014-2026 Fred Gleason <fredg@paravelsystems.com>
##
##    This program is free software; you can redistribute it and/or modify
##    it under the terms of version 2.1 of the GNU Lesser General Public
##    License as published by the Free Software Foundation;
##
##    This program is distributed in the hope that it will be useful,
##    but WITHOUT ANY WARRANTY; without even the implied warranty of
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##    GNU Lesser General Public License for more details.
##
##    You should have received a copy of the GNU General Public License
##    along with this program; if not, write to the Free Software
##    Foundation, Inc., 59 Temple Place, Suite 330, 
##    Boston, MA  02111-1307  USA
##
## Use automake to process this into a Makefile.in

AM_CPPFLAGS = -Wall -Wno-strict-aliasing -std=c++11 -fPIC -I$(top_srcdir) @QT5_CFLAGS@
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

clean-local:	
	rm -rf debug release

lib_LTLIBRARIES = libSwitchyard5.la
dist_libSwitchyard5_la_SOURCES = syadv_packet.cpp syadv_packet.h\
                                 syadv_reader.cpp syadv_reader.h\
                                 syadv_server.cpp syadv_server.h\
                                 syadv_source.cpp syadv_source.h\
                                 syadv_tag.cpp syadv_tag.h\
                                 syastring.cpp syastring.h\
                                 syclock.cpp syclock.h\
                                 sycmdswitch.cpp sycmdswitch.h\
                                 syconfig.h\
                                 sydestination.cpp sydestination.h\
                                 sye2e_message.cpp sye2e_message.h\
                                 sye2e_server.cpp sye2e_server.h\
                                 syendpoint.cpp syendpoint.h\
                                 syethmonitor.cpp syethmonitor.h\
                                 sygpo.cpp sygpo.h\
                                 sygpiobundle.cpp sygpiobundle.h\
                                 sygpio_server.cpp sygpio_server.h\
                                 syinterfaces.cpp syinterfaces.h\
                                 sylo_message.cpp sylo_message.h\
                                 sylo_server.cpp sylo_server.h\
                                 sylwrp_client.cpp\
                                 sylwrp_client.h\
                                 sylwrp_clientconnection.cpp\
                                 sylwrp_clientconnection.h\
                                 sylwrp_server.cpp sylwrp_server.h\
                                 symcastsocket.cpp symcastsocket.h\
                                 symessage.cpp symessage.h\
                                 symeter_message.cpp symeter_message.h\
                                 symeter_server.cpp symeter_server.h\
                                 synode.cpp synode.h\
                                 syprofile.cpp syprofile.h\
                                 sysignalnotifier.cpp sysignalnotifier.h\
                                 sysyslog.cpp sysyslog.h\
                                 syringbuffer.cpp syringbuffer.h\
                                 syrouting.cpp syrouting.h\
                                 syrtp_header.cpp syrtp_header.h\
                                 syrtp_server.cpp syrtp_server.h\
                                 sysource.cpp sysource.h

nodist_libSwitchyard5_la_SOURCES = moc_syadv_server.cpp\
                                  moc_syclock.cpp\
                                  moc_sye2e_server.cpp\
                                  moc_syethmonitor.cpp\
                                  moc_sygpio_server.cpp\
                                  moc_sylo_server.cpp\
                                  moc_sylwrp_client.cpp\
                                  moc_sylwrp_server.cpp\
                                  moc_symcastsocket.cpp\
                                  moc_symeter_server.cpp\
                                  moc_syrtp_server.cpp\
                                  moc_sysignalnotifier.cpp

includedir = $(prefix)/include/sy5
include_HEADERS = syadv_packet.h\
                  syadv_reader.h\
                  syadv_server.h\
                  syadv_source.h\
                  syadv_tag.h\
                  syastring.h\
                  syendpoint.h\
                  syclock.h\
                  sycmdswitch.h\
                  syconfig.h\
                  sydestination.h\
                  sye2e_server.h\
                  syethmonitor.h\
                  sygpiobundle.h\
                  sygpo.h\
                  sygpio_server.h\
                  syinterfaces.h\
                  sylo_server.h\
                  sylwrp_client.h\
                  sylwrp_clientconnection.h\
                  sylwrp_server.h\
                  symcastsocket.h\
                  symessage.h\
                  symeter_message.h\
                  synode.h\
                  syprofile.h\
                  syringbuffer.h\
                  syrouting.h\
                  syrtp_header.h\
                  syrtp_server.h\
                  sysignalnotifier.h\
                  sysource.h

libSwitchyard5_la_LDFLAGS = -version-info $(INTERFACE_CURRENT):$(INTERFACE_REVISION):$(INTERFACE_AGE)

EXTRA_DIST = sy.pro

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             moc_*
MAINTAINERCLEANFILES = *~\
                       Makefile.in
