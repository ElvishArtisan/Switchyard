#!/bin/sh
##
##    (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
##
##    All rights reserved.
##

aclocal $ACLOCAL_FLAGS || {
    echo "aclocal \$ACLOCAL_FLAGS where \$ACLOCAL_FLAGS= failed, exiting..."
    exit 1
}

automake --add-missing || {
    echo "automake --add-missing failed, exiting..."
    exit 1
}

autoconf || {
    echo "autoconf failed, exiting..."
    exit 1
}
