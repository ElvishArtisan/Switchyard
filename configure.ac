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

# to avoid rpath usage :
# http://wiki.debian.net/index.cgi?RpathIssue
#case ${host} in
#  *-pc-linux-gnu)
#    AC_MSG_RESULT([Fixing libtool for -rpath problems.])
#    sed < libtool > libtool-2 \
#    's/^hardcode_libdir_flag_spec.*$/hardcode_libdir_flag_spec=" -D__LIBTOOL_IS_A_FOOL__ "/'
#    mv libtool-2 libtool
#    chmod 755 libtool
#  ;;
#esac

#
# Determine the target architecture
#
AR_GCC_TARGET()
AC_SUBST(VENDOR,$ar_gcc_distro)
AC_SUBST(ARCH,$ar_gcc_arch)

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

AC_CONFIG_FILES([\
    Makefile \
    common/Makefile \
    conf/Makefile \
    helpers/Makefile \
    syd/Makefile \
    syd/rc.syd \
    ])
helpers/link_common.sh syd

AC_OUTPUT()
AC_MSG_NOTICE()
AC_MSG_NOTICE(Now enter 'make' to build the software.)
AC_MSG_NOTICE()
