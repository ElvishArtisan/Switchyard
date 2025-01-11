## Makefile.am
##
## Top level automake.am for Switchyard6
##
## Use automake to process this into a Makefile.in
##
##   (C) Copyright 2014-2022 Fred Gleason <fredg@paravelsystems.com>
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
          debian\
          sy6\
          tests

ACLOCAL_AMFLAGS = -I m4

maintainer-clean-local:	
	rm -rf m4

deb:	dist
	./build_debs.sh

rpm:	dist
	cp switchyard6-$(VERSION).tar.gz $(HOME)/rpmbuild/SOURCES/
	$(RPMBUILD) -ba --target $(ARCH)-$(VENDOR)-linux switchyard6.spec
	mv $(HOME)/rpmbuild/RPMS/$(ARCH)/switchyard6-*.rpm $(top_srcdir)/
	mv $(HOME)/rpmbuild/SRPMS/switchyard6-*.src.rpm $(top_srcdir)/
	rm $(HOME)/rpmbuild/SOURCES/switchyard6-$(VERSION).tar.gz
	rm -rf $(HOME)/rpmbuild/BUILD/switchyard6-$(VERSION)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = switchyard6.pc

EXTRA_DIST = autogen.sh\
             build_debs.sh.in\
             get_distro.sh\
             get_target.sh\
             PACKAGE_VERSION\
             switchyard6.pc.in\
             switchyard6.pro\
             switchyard6.spec.in

CLEANFILES = *~\
             *.buildinfo\
             *.changes\
             *.ddeb\
             *.deb\
             *.dsc\
             *.tar.gz\
             *.tar.xz\
             *.tgz\
             *.mo\
             *.rpm\
             *.zip
DISTCLEANFILES = *.spec\
                 build_debs.sh\
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
