## automake.am
##
## Makefile for AoIP common routines
##
## (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
##
##   All Rights Reserved.
##
## Use automake to process this into a Makefile.in

AM_CPPFLAGS = -Wall -Wno-strict-aliasing -I$(top_srcdir)
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
                                sycmdswitch.cpp sycmdswitch.h\
                                syconfig.h\
                                sygpio_server.cpp sygpio_server.h\
                                sylogger.cpp sylogger.h\
                                sylwrp_clientconnection.cpp\
                                sylwrp_clientconnection.h\
                                sylwrp_server.cpp sylwrp_server.h\
                                symcastsocket.cpp symcastsocket.h\
                                syprofile.cpp syprofile.h\
                                syringbuffer.cpp syringbuffer.h\
                                syrouting.cpp syrouting.h\
                                syrtp_header.cpp syrtp_header.h\
                                syrtp_server.cpp syrtp_server.h

nodist_libSwitchyard_la_SOURCES = moc_syadv_server.cpp\
                                  moc_sygpio_server.cpp\
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
                  sycmdswitch.h\
                  syconfig.h\
                  sygpio_server.h\
                  sylwrp_clientconnection.h\
                  sylwrp_server.h\
                  symcastsocket.h\
                  syprofile.h\
                  syringbuffer.h\
                  syrouting.h\
                  syrtp_header.h\
                  syrtp_server.h

libSwitchyard_la_LDFLAGS = -version-info $(INTERFACE_CURRENT):$(INTERFACE_REVISION):$(INTERFACE_AGE)

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             moc_*
MAINTAINERCLEANFILES = *~\
                       Makefile.in
