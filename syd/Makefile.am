## automake.am
##
## Switchyard Control Daemon
##
## (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
##
##   All Rights Reserved.
##
## Use automake to process this into a Makefile.in

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -pthread -Wno-strict-aliasing
INCLUDES = -I$(top_srcdir)/common
LIBS = -L$(top_srcdir)/common
MOC = @MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@


sbin_PROGRAMS = syd

dist_syd_SOURCES = rtp_callback.cpp\
                   syd.cpp syd.h

nodist_syd_SOURCES = adv_astring.cpp adv_astring.h\
                     adv_packet.cpp adv_packet.h\
                     adv_source.cpp adv_source.h\
                     adv_tag.cpp adv_tag.h\
                     cmdswitch.cpp cmdswitch.h\
                     config.h\
                     lwrp_clientconnection.cpp lwrp_clientconnection.h\
                     lwrp_server.cpp lwrp_server.h\
                     profile.cpp profile.h\
                     routing.cpp routing.h\
                     rtp.cpp rtp.h\
                     moc_lwrp_server.cpp\
                     moc_rtp.cpp\
                     moc_syd.cpp

syd_LDADD = -lQtCore -lQtNetwork
syd_LDFLAGS = -pthread

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             moc_*
MAINTAINERCLEANFILES = *~\
                       Makefile.in\
                       adv_astring.cpp adv_astring.h\
                       adv_packet.cpp adv_packet.h\
                       adv_source.cpp adv_source.h\
                       adv_tag.cpp adv_tag.h\
                       cmdswitch.cpp cmdswitch.h\
                       config.h\
                       lwrp_clientconnection.cpp lwrp_clientconnection.h\
                       lwrp_server.cpp lwrp_server.h\
                       profile.cpp profile.h\
                       routing.cpp routing.h\
                       rtp.cpp rtp.h
