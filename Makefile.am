## automake.am
##
## Top level automake.am for Switchyard5
##
## Use automake to process this into a Makefile.in
##
##   (C) Copyright 2014-2021 Fred Gleason <fredg@paravelsystems.com>
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

SUBDIRS = conf\
          sy5\
          tests

ACLOCAL_AMFLAGS = -I m4

maintainer-clean-local:	
	rm -rf m4

rpm:	dist
	cp switchyard5-$(VERSION).tar.gz $(HOME)/rpmbuild/SOURCES/
	$(RPMBUILD) -ba --target $(ARCH)-$(VENDOR)-linux switchyard5.spec
	mv $(HOME)/rpmbuild/RPMS/$(ARCH)/switchyard5-*.rpm $(top_srcdir)/
	mv $(HOME)/rpmbuild/SRPMS/switchyard5-*.src.rpm $(top_srcdir)/
	rm $(HOME)/rpmbuild/SOURCES/switchyard5-$(VERSION).tar.gz
	rm -rf $(HOME)/rpmbuild/BUILD/switchyard5-$(VERSION)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = switchyard5.pc

EXTRA_DIST = autogen.sh\
             get_distro.sh\
             get_target.sh\
             PACKAGE_VERSION\
             switchyard5.pc.in\
             switchyard5.pro\
             switchyard5.spec.in

CLEANFILES = *~\
             *.tar.gz\
             *.tgz\
             *.mo\
             *.rpm\
             *.zip
DISTCLEANFILES = *.spec\
                 config.guess\
                 config.sub\
                 rc.syd
MAINTAINERCLEANFILES = *~\
                       *.pc\
                       *.tar.gz\
                       *.spec\
                       aclocal.m4\
                       compile\
                       configure\
                       config.guess\
                       config.status\
                       config.sub\
                       depcomp\
                       install-sh\
                       ltmain.sh\
                       Makefile.in\
                       missing\
                       mkinstalldirs
