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

SUBDIRS = conf syd

install-exec-local:	
	mkdir -p /etc/init.d
	cp rc.syd /etc/init.d/syd

uninstall-exec:
	rm -f /etc/init.d/syd

EXTRA_DIST = autogen.sh\
             get_distro.sh\
             get_target.sh\
             PACKAGE_VERSION\
             rc.syd.in

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
                       *.tar.gz\
                       *.spec\
                       aclocal.m4\
                       configure\
                       config.guess\
                       config.status\
                       config.sub\
                       depcomp\
                       install-sh\
                       Makefile.in\
                       missing\
                       mkinstalldirs
