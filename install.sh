#!/bin/bash

ARCH=`uname -m`

if [[ $ARCH == "arm6"* ]]; then export "VIS_COMPILER_ARCH"="armv6"; fi
if [[ $ARCH == "armv6"* ]]; then export "VIS_COMPILER_ARCH"="armv6"; fi
if [[ $ARCH == "armv7"* ]]; then export "VIS_COMPILER_ARCH"="armv6"; fi

mkdir build/
cd build/
cmake ../ && make clean && make -j$(nproc) && sudo make install
cd ..

if [ -z "$XDG_CONFIG_HOME" ]
then
    CONFIG_DIR=$HOME/.config/vis
else
    CONFIG_DIR=$XDG_CONFIG_HOME/vis
fi

#create config directory
mkdir -p "$CONFIG_DIR/colors"

#copy over example files
cp examples/config $CONFIG_DIR/
cp examples/rainbow $CONFIG_DIR/colors/rainbow
cp examples/basic_colors $CONFIG_DIR/colors/basic_colors
