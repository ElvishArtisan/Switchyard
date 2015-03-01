## automake.am
##
## Switchyard Control Daemon
##
## (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
##
##   All Rights Reserved.
##
## Use automake to process this into a Makefile.in

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -pthread -I$(top_srcdir)
LIBS = -L$(top_srcdir)/sy
MOC = @MOC@

#install-exec-local:	
#	mkdir -p ${DESTDIR}/etc/init.d
#	cp rc.syd ${DESTDIR}/etc/init.d/syd
#	chmod 755 ${DESTDIR}/etc/init.d/syd

#uninstall-local:
#	rm -f ${DESTDIR}/etc/init.d/syd

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@


noinst_PROGRAMS = sycapture\
                  syd\
                  syplay

dist_syd_SOURCES = syd.cpp syd.h
nodist_syd_SOURCES = moc_syd.cpp
syd_LDADD = @SWITCHYARDLIBS@
syd_LDFLAGS = @SWITCHYARDFLAGS@

dist_sycapture_SOURCES = sycapture.cpp sycapture.h
nodist_sycapture_SOURCES = moc_sycapture.cpp
sycapture_LDADD = @SWITCHYARDLIBS@ @libsndfile_LIBS@
sycapture_LDFLAGS = @SWITCHYARDFLAGS@ @libsndfile_CFLAGS@

dist_syplay_SOURCES = syplay.cpp syplay.h
nodist_syplay_SOURCES = moc_syplay.cpp
syplay_LDADD = @SWITCHYARDLIBS@ @libsndfile_LIBS@
syplay_LDFLAGS = @SWITCHYARDFLAGS@ @libsndfile_CFLAGS@

EXTRA_DIST = rc.syd.in

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             moc_*

DISTCLEANFILES = rc.syd

MAINTAINERCLEANFILES = *~\
                       Makefile.in
