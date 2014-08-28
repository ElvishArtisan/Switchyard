REM @ECHO OFF

REM build_win32.bat
REM
REM Build Switchyard for Win32.
REM
REM (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
REM     All Rights Reserved.
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
