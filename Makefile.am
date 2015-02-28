## automake.am
##
## Top level automake.am for Switchyard
##
## Use automake to process this into a Makefile.in
##
##   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
##
##   All Rights Reserved.
##

SUBDIRS = conf\
          sy\
          tests

ACLOCAL_AMFLAGS = -I m4

maintainer-clean-local:	
	rm -rf m4

rpm:	dist
	cp switchyard-$(VERSION).tar.gz $(HOME)/rpmbuild/SOURCES/
	$(RPMBUILD) -ba --target $(ARCH)-$(VENDOR)-linux switchyard.spec
	mv $(HOME)/rpmbuild/RPMS/$(ARCH)/switchyard-*.rpm $(top_srcdir)/
	mv $(HOME)/rpmbuild/SRPMS/switchyard-*.src.rpm $(top_srcdir)/
	rm $(HOME)/rpmbuild/SOURCES/switchyard-$(VERSION).tar.gz
	rm -rf $(HOME)/rpmbuild/BUILD/switchyard-$(VERSION)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = Switchyard.pc

EXTRA_DIST = autogen.sh\
             get_distro.sh\
             get_target.sh\
             PACKAGE_VERSION\
             Switchyard.pc.in\
             switchyard.pro\
             switchyard.spec.in

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
