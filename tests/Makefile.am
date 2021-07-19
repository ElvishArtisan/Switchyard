## automake.am
##
## Switchyard Control Daemon
##
## (C) Copyright 2014-2021 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -pthread -I$(top_srcdir) -std=c++11 -fPIC @QT5_CFLAGS@ @QT5GUI_CFLAGS@
LIBS = -L$(top_srcdir)/sy5 @QT5_LIBS@
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@


noinst_PROGRAMS = codeinverttest\
                  syastringtest\
                  sycapture\
                  syd\
                  syethmonitortest\
                  syinterfacestest\
                  symetertest\
                  synodetest\
                  syplay\
                  sytone

bin_PROGRAMS = sygpiotest

dist_codeinverttest_SOURCES = codeinverttest.cpp codeinverttest.h
nodist_codeinverttest_SOURCES = moc_codeinverttest.cpp
codeinverttest_LDADD = @SWITCHYARDLIBS@
codeinverttest_LDFLAGS = @SWITCHYARDFLAGS@

dist_syastringtest_SOURCES = syastringtest.cpp syastringtest.h
nodist_syastringtest_SOURCES = moc_syastringtest.cpp
syastringtest_LDADD = @SWITCHYARDLIBS@
syastringtest_LDFLAGS = @SWITCHYARDFLAGS@

dist_syd_SOURCES = syd.cpp syd.h
nodist_syd_SOURCES = moc_syd.cpp
syd_LDADD = @SWITCHYARDLIBS@
syd_LDFLAGS = @SWITCHYARDFLAGS@

dist_sycapture_SOURCES = sycapture.cpp sycapture.h
nodist_sycapture_SOURCES = moc_sycapture.cpp
sycapture_LDADD = @SWITCHYARDLIBS@ @libsndfile_LIBS@
sycapture_LDFLAGS = @SWITCHYARDFLAGS@ @libsndfile_CFLAGS@

dist_syethmonitortest_SOURCES = syethmonitortest.cpp syethmonitortest.h
nodist_syethmonitortest_SOURCES = moc_syethmonitortest.cpp
syethmonitortest_LDADD = @SWITCHYARDLIBS@
syethmonitortest_LDFLAGS = @SWITCHYARDFLAGS@

dist_syinterfacestest_SOURCES = syinterfacestest.cpp syinterfacestest.h
nodist_syinterfacestest_SOURCES = moc_syinterfacestest.cpp
syinterfacestest_LDADD = @SWITCHYARDLIBS@
syinterfacestest_LDFLAGS = @SWITCHYARDFLAGS@

dist_sygpiotest_SOURCES = sygpiotest.cpp sygpiotest.h
nodist_sygpiotest_SOURCES = moc_sygpiotest.cpp
sygpiotest_LDADD = @SWITCHYARDLIBS@
sygpiotest_LDFLAGS = @SWITCHYARDFLAGS@

dist_symetertest_SOURCES = playmeter.cpp playmeter.h\
                           statuslight.cpp statuslight.h\
                           symetertest.cpp symetertest.h
nodist_symetertest_SOURCES = moc_playmeter.cpp\
                             moc_statuslight.cpp\
                             moc_symetertest.cpp
symetertest_LDADD = @SWITCHYARDLIBS@ @QT5GUI_LIBS@
symetertest_LDFLAGS = @SWITCHYARDFLAGS@

dist_synodetest_SOURCES = synodetest.cpp synodetest.h
nodist_synodetest_SOURCES = moc_synodetest.cpp
synodetest_LDADD = @SWITCHYARDLIBS@
synodetest_LDFLAGS = @SWITCHYARDFLAGS@

dist_syplay_SOURCES = syplay.cpp syplay.h
nodist_syplay_SOURCES = moc_syplay.cpp
syplay_LDADD = @SWITCHYARDLIBS@ @libsndfile_LIBS@
syplay_LDFLAGS = @SWITCHYARDFLAGS@ @libsndfile_CFLAGS@

dist_sytone_SOURCES = sytone.cpp sytone.h
nodist_sytone_SOURCES = moc_sytone.cpp
sytone_LDADD = @SWITCHYARDLIBS@ -lm
sytone_LDFLAGS = @SWITCHYARDFLAGS@

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
