## automake.am
##
## Makefile for AoIP common routines
##
## (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -Wno-strict-aliasing -I$(top_srcdir) @QT4_CFLAGS@
MOC = @MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

clean-local:	
	rm -rf debug release

lib_LTLIBRARIES = libSwitchyard.la
dist_libSwitchyard_la_SOURCES = syadv_packet.cpp syadv_packet.h\
                                syadv_reader.cpp syadv_reader.h\
                                syadv_server.cpp syadv_server.h\
                                syadv_source.cpp syadv_source.h\
                                syadv_tag.cpp syadv_tag.h\
                                syastring.cpp syastring.h\
                                syclock.cpp syclock.h\
                                sycmdswitch.cpp sycmdswitch.h\
                                syconfig.h\
                                sydestination.cpp sydestination.h\
                                syendpoint.cpp syendpoint.h\
                                syethmonitor.cpp syethmonitor.h\
                                sygpo.cpp sygpo.h\
                                sygpiobundle.cpp sygpiobundle.h\
                                sygpio_server.cpp sygpio_server.h\
                                syinterfaces.cpp syinterfaces.h\
                                sylwrp_client.cpp\
                                sylwrp_client.h\
                                sylwrp_clientconnection.cpp\
                                sylwrp_clientconnection.h\
                                sylwrp_server.cpp sylwrp_server.h\
                                symcastsocket.cpp symcastsocket.h\
                                synode.cpp synode.h\
                                syprofile.cpp syprofile.h\
                                sysyslog.cpp sysyslog.h\
                                syringbuffer.cpp syringbuffer.h\
                                syrouting.cpp syrouting.h\
                                syrtp_header.cpp syrtp_header.h\
                                syrtp_server.cpp syrtp_server.h\
                                sysource.cpp sysource.h

nodist_libSwitchyard_la_SOURCES = moc_syadv_server.cpp\
                                  moc_syclock.cpp\
                                  moc_syethmonitor.cpp\
                                  moc_sygpio_server.cpp\
                                  moc_sylwrp_client.cpp\
                                  moc_sylwrp_server.cpp\
                                  moc_symcastsocket.cpp\
                                  moc_syrtp_server.cpp

includedir = $(prefix)/include/sy
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
                  syethmonitor.h\
                  sygpiobundle.h\
                  sygpo.h\
                  sygpio_server.h\
                  syinterfaces.h\
                  sylwrp_client.h\
                  sylwrp_clientconnection.h\
                  sylwrp_server.h\
                  symcastsocket.h\
                  synode.h\
                  syprofile.h\
                  syringbuffer.h\
                  syrouting.h\
                  syrtp_header.h\
                  syrtp_server.h\
                  sysource.h

libSwitchyard_la_LDFLAGS = -version-info $(INTERFACE_CURRENT):$(INTERFACE_REVISION):$(INTERFACE_AGE)

EXTRA_DIST = sy.pro

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             moc_*
MAINTAINERCLEANFILES = *~\
                       Makefile.in
