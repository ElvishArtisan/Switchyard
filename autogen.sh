#!/bin/sh
##
##    (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
##
##    All rights reserved.
##

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
