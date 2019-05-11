#!/bin/bash

TEMP_DIRECTORY="temp";

if [ ! -d "$TEMP_DIRECTORY" ]; then
	mkdir $TEMP_DIRECTORY;
fi

cd $TEMP_DIRECTORY
git clone https://github.com/oktal/pistache.git
git submodule update --init
cd pistache
mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make
sudo make install
