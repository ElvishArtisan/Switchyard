dnl configure.ac
dnl
dnl Use autoconf to process this into a configure script
dnl
dnl   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com dnl
dnl
dnl   All Rights Reserved.
dnl

AC_INIT(switchyard,`cat PACKAGE_VERSION`,Fred Gleason <fredg@paravelsystems.com>)
AM_INIT_AUTOMAKE(switchyard,`cat PACKAGE_VERSION`)
AC_SUBST(RPM_RELEASE,1)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

#
# Some Fully Resolved Install Paths
# (for the packaging system and friends)
#
if test ${prefix} = "NONE" ; then
  AC_SUBST(LOCAL_PREFIX,/usr/local)
else
  AC_SUBST(LOCAL_PREFIX,${prefix})
fi

#
# Basic Compiler Checks
#
AC_PROG_CXX
AC_LANG(C++)

#
# Determine the target architecture
#
AR_GCC_TARGET()
AC_SUBST(VENDOR,$ar_gcc_distro)
AC_SUBST(ARCH,$ar_gcc_arch)
if test $ar_gcc_distro = "apple" ; then
   AC_MSG_RESULT([OS X.])
   AC_DEFINE(OSX)
else
   AC_MSG_RESULT([Linux.])
   AC_DEFINE(LINUX)
fi

#
# Check for Qt4 (Mandatory)
#
AC_CHECK_HEADER(QtCore/QtCore,[],[AC_MSG_ERROR([*** Qt4 not found ***])])
AC_CHECK_PROG(MOC_NAME,moc-qt4,[moc-qt4],[moc])
AC_SUBST(MOC,$MOC_NAME)

#
# Determine Distro
#
AR_GET_DISTRO()
AC_SUBST(DISTRO,$ar_gcc_distro)
if test $ar_gcc_distro = suse ; then
  AC_SUBST(PACKAGE_DOC_DIR,"/usr/share/doc/packages/$PACKAGE_NAME")
else
  AC_SUBST(PACKAGE_DOC_DIR,"/usr/share/doc/$PACKAGE_NAME-`cat PACKAGE_VERSION`")
fi

#
# Configure Linker Flags
#
if test $ar_gcc_distro = "apple" ; then
  AC_SUBST(SWITCHYARDLIBS,"")
  AC_SUBST(SWITCHYARDFLAGS,"-framework QtCore -framework QtNetwork")
else
  AC_SUBST(SWITCHYARDLIBS,"-lQtCore -lQtNetwork")
  AC_SUBST(SWITCHYARDFLAGS,"-pthread")
fi

AC_CONFIG_FILES([\
    Makefile \
    common/Makefile \
    conf/Makefile \
    syd/Makefile \
    syd/rc.syd \
    ])
common/link_common.sh syd

AC_OUTPUT()
AC_MSG_NOTICE()
AC_MSG_NOTICE(Now enter 'make' to build the software.)
AC_MSG_NOTICE()
