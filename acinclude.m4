dnl acinclude.m4
dnl
dnl Local Autoconf macro definitions
dnl
dnl   (C) Copyright 2006 Fred Gleason <fredg@paravelsystems.com>
dnl
dnl   All Rights Reserved.
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

dnl AR_GCC_TARGET()
dnl
dnl Get gcc(1)'s idea of the target architecture, distribution and os.
dnl The following variables are set:
dnl   $ar_gcc_arch = Target Architecture (i586, XF86_64, etc)
dnl   $ar_gcc_distro = Target Distribution (suse, slackware, etc)
dnl   $ar_gcc_os = Target Operating System (linux, solaris, etc)
dnl
AC_DEFUN([AR_GCC_TARGET],[AC_REQUIRE([AC_PROG_CC])]
  [
  AC_MSG_CHECKING(target architecture)
  ar_gcc_arch=$(./get_target.sh $CC $AWK arch)
  ar_gcc_distro=$(./get_target.sh $CC $AWK distro)
  ar_gcc_os=$(./get_target.sh $CC $AWK os)
  AC_MSG_RESULT([$ar_gcc_arch-$ar_gcc_distro-$ar_gcc_os])
  ]
)

dnl AR_GET_DISTRO()
dnl
dnl Try to determine the name and version of the distribution running
dnl on the host machine, based on entries in '/etc/'.
dnl The following variables are set:
dnl   $ar_distro_name = Distribution Name (SuSE, Debian, etc)
dnl   $ar_distro_version = Distribution Version (10.3, 3.1, etc)
dnl
AC_DEFUN([AR_GET_DISTRO],[]
  [
  AC_MSG_CHECKING(distribution)
  ar_distro_name=$(./get_distro.sh NAME $AWK)
  ar_distro_version=$(./get_distro.sh VERSION $AWK)
  AC_MSG_RESULT([$ar_distro_name $ar_distro_version])
  ]
)
