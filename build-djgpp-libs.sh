## ========================================================
##
## FILE:			/build-djgpp-libs.sh
##
## AUTHORS:			Jaycie Ewald
##
## PROJECT:			libcohost
##
## LICENSE:			ACSL v1.4
##
## DESCRIPTION:		Bash script for building the thirdparty
##					libraries with DJGPP
##
## LAST EDITED:		November 23rd, 2022
##
## ========================================================

#!/bin/bash

## build watt32
export WATT_ROOT=./thirdparty/watt
cd ./thirdparty/watt/src
./configur.sh djgpp
wine ../util/win32/dj_err.exe -s > ./build/djgpp/syserr.c
wine ../util/win32/dj_err.exe -e > ../inc/sys/djgpp.err
make -f djgpp.mak
cp ../lib/libwatt.a ../../../libs/libwatt.a

## build libcurl
cd ../../curl/lib
make -f Makefile.mk CFG=djgpp CROSSPREFIX=i586-pc-msdosdjgpp- WATT_PATH=../../watt
cp libcurl.a ../../../libs/libcurl.a

## build cjson
cd ../../cjson
i586-pc-msdosdjgpp-gcc -c cJSON.c
i586-pc-msdosdjgpp-ar rcs libcjson.a cJSON.o
cp libcjson.a ../../libs/libcjson.a
