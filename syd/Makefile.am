## automake.am
##
## Switchyard Control Daemon
##
## (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
##
##   All Rights Reserved.
##
## Use automake to process this into a Makefile.in

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -pthread -Wno-strict-aliasing -I$(top_srcdir)/common
LIBS = -L$(top_srcdir)/common
MOC = @MOC@

install-exec-local:	
	mkdir -p ${DESTDIR}/etc/init.d
	cp rc.syd ${DESTDIR}/etc/init.d/syd
	chmod 755 ${DESTDIR}/etc/init.d/syd

uninstall-local:
	rm -f ${DESTDIR}/etc/init.d/syd

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@


sbin_PROGRAMS = syd

dist_syd_SOURCES = rtp_callback.cpp\
                   syd.cpp syd.h

nodist_syd_SOURCES = adv_packet.cpp adv_packet.h\
                     adv_reader.cpp adv_reader.h\
                     adv_server.cpp adv_server.h\
                     adv_source.cpp adv_source.h\
                     adv_tag.cpp adv_tag.h\
                     astring.cpp astring.h\
                     cmdswitch.cpp cmdswitch.h\
                     config.h\
                     lwrp_clientconnection.cpp lwrp_clientconnection.h\
                     lwrp_server.cpp lwrp_server.h\
                     profile.cpp profile.h\
                     routing.cpp routing.h\
                     rtp.cpp rtp.h\
                     moc_adv_server.cpp\
                     moc_lwrp_server.cpp\
                     moc_rtp.cpp\
                     moc_syd.cpp

syd_LDADD = @SWITCHYARDLIBS@
syd_LDFLAGS = @SWITCHYARDFLAGS@

EXTRA_DIST = rc.syd.in

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             moc_*

DISTCLEANFILES = rc.syd

MAINTAINERCLEANFILES = *~\
                       Makefile.in\
                       adv_astring.cpp adv_astring.h\
                       adv_packet.cpp adv_packet.h\
                       adv_reader.cpp adv_reader.h\
                       adv_server.cpp adv_server.h\
                       adv_source.cpp adv_source.h\
                       adv_tag.cpp adv_tag.h\
                       astring.cpp astring.h\
                       cmdswitch.cpp cmdswitch.h\
                       config.h\
                       lwrp_clientconnection.cpp lwrp_clientconnection.h\
                       lwrp_server.cpp lwrp_server.h\
                       profile.cpp profile.h\
                       routing.cpp routing.h\
                       rtp.cpp rtp.h
