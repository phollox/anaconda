#!/bin/sh

GAMEDIR=`dirname "$(readlink -f "$0")"`
cd "$GAMEDIR"

MACHINE_TYPE=`uname -m`
if [ ${MACHINE_TYPE} == 'x86_64' ]; then
    ./bin64/Chowdren
else
    ./bin32/Chowdren
  # 32-bit stuff here
fi
