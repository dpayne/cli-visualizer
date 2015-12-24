#!/bin/bash

make clean
make
sudo make install

#create .vis directory
mkdir -p ~/.vis/colors

#copy over example files
cp examples/config ~/.vis/
cp examples/rainbow ~/.vis/colors/rainbow
