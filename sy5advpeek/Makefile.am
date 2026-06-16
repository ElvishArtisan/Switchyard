## Makefile.am
##
## Makefile.am for sy5advpeek(1)
##
## (C) Copyright 2026 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -pthread -I$(top_srcdir) -std=c++11 -fPIC @QT5_CFLAGS@ @QT5_CFLAGS@
LIBS = -L$(top_srcdir)/sy5 @QT5_LIBS@
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@


bin_PROGRAMS = sy5advpeek

dist_sy5advpeek_SOURCES = sy5advpeek.cpp sy5advpeek.h
nodist_sy5advpeek_SOURCES = moc_sy5advpeek.cpp
sy5advpeek_LDADD = @SWITCHYARDLIBS@
sy5advpeek_LDFLAGS = @SWITCHYARDFLAGS@

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             moc_*

DISTCLEANFILES = rc.syd

MAINTAINERCLEANFILES = *~\
                       Makefile.in
