dnl configure.ac
dnl
dnl Use autoconf to process this into a configure script
dnl
dnl   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com dnl
dnl
dnl   All Rights Reserved.
dnl

AC_INIT(switchyard,m4_esyscmd(cat PACKAGE_VERSION))
AM_INIT_AUTOMAKE
AC_SUBST(RPM_RELEASE,1)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AC_CONFIG_MACRO_DIR([m4])

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
AC_PROG_LIBTOOL
AC_LANG(C++)

#
# Determine Library API Versioning
#
# From http://www.gnu.org/software/libtool/manual.html#Updating-version-info
#
#   1. Start with version information of 0:0:0 for each libtool library.
#   2. Update the version information only immediately before a public
#      release of your software. More frequent updates are unnecessary,
#      and only guarantee that the current interface number gets larger
#      faster.
#   3. If the library source code has changed at all since the last update,
#      then increment 'REVISION' (c:r:a becomes c:r+1:a).
#   4. If any interfaces have been added, removed, or changed since the last
#      update, increment 'CURRENT', and set 'REVISION' to 0.
#   5. If any interfaces have been added since the last public release, then
#      increment 'AGE'.
#   6. If any interfaces have been removed since the last public release,
#      then set 'AGE' to 0.
CURRENT=5
REVISION=3
AGE=2
POINT=$(($CURRENT - $AGE))

#
# Calculate the Library Version Values
#
AC_SUBST(INTERFACE_CURRENT,$CURRENT)
AC_SUBST(INTERFACE_REVISION,$REVISION)
AC_SUBST(INTERFACE_AGE,$AGE)
AC_SUBST(INTERFACE_MAJOR,$POINT)
AC_SUBST(INTERFACE_MINOR,$AGE)
AC_SUBST(INTERFACE_POINT,$REVISION)

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
PKG_CHECK_MODULES(QT4,QtCore QtNetwork,[],[AC_MSG_ERROR([*** Qt4 not found ***])])
PKG_CHECK_MODULES(QT4GUI,QtGui,[],[AC_MSG_ERROR([*** Qt4 GUI not found ***])])
AC_CHECK_PROG(MOC_NAME,moc-qt4,[moc-qt4],[moc])
AC_SUBST(MOC,$MOC_NAME)

#
# Check for libsndfile (Needed for sycapture)
#
PKG_CHECK_MODULES(libsndfile,[sndfile],[],[AC_MSG_ERROR([*** libsndfile not found ***])])

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
  AC_SUBST(SWITCHYARDLIBS,"-lSwitchyard")
  AC_SUBST(SWITCHYARDFLAGS,"-framework QtCore -framework QtNetwork")
else
  AC_SUBST(SWITCHYARDLIBS,"-lSwitchyard -lQtCore -lQtNetwork")
  AC_SUBST(SWITCHYARDFLAGS,"-pthread")
fi

#
# Configure RPM Build
#
AC_CHECK_PROG(RPMBUILD_FOUND,rpmbuild,[yes],[])
if test -z $RPMBUILD_FOUND ; then
  AC_SUBST(RPMBUILD,rpm)
else
  AC_SUBST(RPMBUILD,rpmbuild)
fi
if test -d /usr/src/redhat ; then
  AC_SUBST(RPM_ROOT,/usr/src/redhat)
  AC_SUBST(VENDOR,redhat)
else
  AC_SUBST(RPM_ROOT,/usr/src/packages)
  AC_SUBST(VENDOR,suse)
fi 

AC_CONFIG_FILES([\
    Makefile \
    sy/Makefile \
    conf/Makefile \
    tests/Makefile \
    tests/rc.syd \
    switchyard.pc \
    switchyard.spec \
    ])

AC_OUTPUT()
AC_MSG_NOTICE()
AC_MSG_NOTICE(Now enter 'make' to build the software.)
AC_MSG_NOTICE()
