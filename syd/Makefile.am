## automake.am
##
## Switchyard Control Daemon
##
## (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
##
##   All Rights Reserved.
##
## Use automake to process this into a Makefile.in

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\"
INCLUDES = -I$(top_srcdir)/common
LIBS = -L$(top_srcdir)/common
MOC = @MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@


sbin_PROGRAMS = syd

dist_syd_SOURCES = cmdswitch.cpp cmdswitch.h\
                   config.h\
                   profile.cpp profile.h\
                   routing.cpp routing.h\
                   syd.cpp syd.h

nodist_syd_SOURCES = moc_syd.cpp

syd_LDADD = -lQtCore -lQtNetwork

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             moc_*
MAINTAINERCLEANFILES = *~\
                       Makefile.in
