#!/bin/sh
##
##    (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#
# Generate Debian packaging metadata
#
DATESTAMP=`date +%a,\ %d\ %b\ %Y\ %T\ %z`
sed s/@VERSION@/`cat PACKAGE_VERSION`/ < debian/control.src > debian/control
sed s/@VERSION@/`cat PACKAGE_VERSION`/ < debian/changelog.src | sed "s/@DATESTAMP@/$DATESTAMP/" > debian/changelog

mkdir -p m4

libtoolize=libtoolize
if which glibtoolize > /dev/null 2>&1; then
      libtoolize=glibtoolize
fi

$libtoolize --force 2>&1 | sed '/^You should/d' || {
    echo "libtool failed, exiting..."
    exit 1
}

aclocal $ACLOCAL_FLAGS || {
    echo "aclocal \$ACLOCAL_FLAGS where \$ACLOCAL_FLAGS= failed, exiting..."
    exit 1
}

automake --add-missing -Wno-portability || {
    echo "automake --add-missing failed, exiting..."
    exit 1
}

autoconf || {
    echo "autoconf failed, exiting..."
    exit 1
}
