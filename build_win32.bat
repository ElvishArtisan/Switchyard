REM @ECHO OFF

REM build_win32.bat
REM
REM Build Switchyard for Win32.
REM
REM (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
REM
REM    This program is free software; you can redistribute it and/or modify
REM    it under the terms of version 2.1 of the GNU Lesser General Public
REM    License as published by the Free Software Foundation;
REM
REM    This program is distributed in the hope that it will be useful,
REM    but WITHOUT ANY WARRANTY; without even the implied warranty of
REM    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM    GNU Lesser General Public License for more details.
REM
REM    You should have received a copy of the GNU General Public License
REM    along with this program; if not, write to the Free Software
REM    Foundation, Inc., 59 Temple Place, Suite 330, 
REM    Boston, MA  02111-1307  USA
REM

REM
REM Stupid DOS trick to get the package version
REM
copy helpers\win32_frag1.txt + package_version + helpers\win32_frag2.txt helpers\win32_version.bat
call helpers\win32_version.bat
del helpers\win32_version.bat

REM
REM Build It
REM
qmake -o Makefile switchyard.pro
make


REM End of build_win32.bat
