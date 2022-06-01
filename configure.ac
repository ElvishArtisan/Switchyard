dnl configure.ac
dnl
dnl Use autoconf to process this into a configure script
dnl
dnl   (C) Copyright 2014-2022 Fred Gleason <fredg@paravelsystems.com dnl
dnl
dnl    This program is free software; you can redistribute it and/or modify
dnl    it under the terms of version 2.1 of the GNU Lesser General Public
dnl    License as published by the Free Software Foundation;
dnl
dnl    This program is distributed in the hope that it will be useful,
dnl    but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl    GNU Lesser General Public License for more details.
dnl
dnl    You should have received a copy of the GNU General Public License
dnl    along with this program; if not, write to the Free Software
dnl    Foundation, Inc., 59 Temple Place, Suite 330, 
dnl    Boston, MA  02111-1307  USA
dnl

AC_INIT(switchyard5,m4_esyscmd(cat PACKAGE_VERSION))
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
CURRENT=17
REVISION=1
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
# Check for Qt5 (Mandatory)
#
PKG_CHECK_MODULES(QT5,Qt5Core Qt5Network,[],[AC_MSG_ERROR([*** Qt5 not found ***])])
PKG_CHECK_MODULES(QT5GUI,Qt5Gui Qt5Widgets,[],[AC_MSG_ERROR([*** Qt5 GUI not found ***])])
AC_CHECK_PROG(MOC_NAME,moc-qt5,[moc-qt5],[moc])
AC_SUBST(QT_MOC,$MOC_NAME)

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
  AC_SUBST(SWITCHYARDLIBS,"-lSwitchyard5")
  AC_SUBST(SWITCHYARDFLAGS,"-framework Qt5Core -framework QtNetwork")
else
  AC_SUBST(SWITCHYARDLIBS,"-lSwitchyard5 -lQt5Core -lQt5Network")
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
AC_SUBST(RPM_DATESTAMP,`date +%a\ %b\ %d\ %Y`)

AC_CONFIG_FILES([\
    Makefile \
    debian/Makefile \
    debian/source/Makefile \
    sy5/Makefile \
    conf/Makefile \
    tests/Makefile \
    tests/rc.syd \
    switchyard5.pc \
    switchyard5.spec \
    build_debs.sh \
    ])
AC_OUTPUT()
chmod 755 build_debs.sh

AC_MSG_NOTICE()
AC_MSG_NOTICE(Now enter 'make' to build the software.)
AC_MSG_NOTICE()
