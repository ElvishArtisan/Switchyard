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

rpm:	dist
	cp switchyard-$(VERSION).tar.gz $(RPM_ROOT)/SOURCES/
	$(RPMBUILD) -ba --target $(ARCH)-$(VENDOR)-linux switchyard.spec
	mv $(RPM_ROOT)/RPMS/$(ARCH)/switchyard-*.rpm $(top_srcdir)/
	mv $(RPM_ROOT)/SRPMS/switchyard-*.src.rpm $(top_srcdir)/
	rm $(RPM_ROOT)/SOURCES/switchyard-$(VERSION).tar.gz
	rm -rf $(RPM_ROOT)/BUILD/switchyard-$(VERSION)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = Switchyard.pc

EXTRA_DIST = autogen.sh\
             get_distro.sh\
             get_target.sh\
             PACKAGE_VERSION\
             Switchyard.pc.in\
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
