#!/bin/bash

# get_target.sh
# 
# Return machine target information from gcc(1)
# Used as part of the AR_GCC_TARGET() macro.
#
#   (C) Copyright 2012 Fred Gleason <fredg@salemradiolabs.com>
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of version 2.1 of the GNU Lesser General Public
#    License as published by the Free Software Foundation;
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, 
#    Boston, MA  02111-1307  USA
#

$1 -dumpmachine > ./gcc_arch

case "$3" in
    arch)
	$2 -F- "// {print \$1}" ./gcc_arch
    ;;
    distro)
	$2 -F- "// {print \$2}" ./gcc_arch
    ;;
    os)
	$2 -F- "// {print \$3}" ./gcc_arch
    ;;
esac

rm -f ./gcc_arch


# End of get_target.sh
