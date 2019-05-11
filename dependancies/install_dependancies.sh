#!/bin/bash 


TEMP_DIRECTORY="./dependancies/temp";

sudo apt-get update
sudo apt-get install curl
sudo apt-get install libboost-all-dev
sudo apt-get install libcurlpp-dev libcurlpp0 libcurl4-gnutls-dev zbar-tools zbar-dbg libzbar-dev
pip3 install --upgrade opencv-python

if [ ! -d "$TEMP_DIRECTORY" ]; then
	mkdir $TEMP_DIRECTORY;
fi

cd $TEMP_DIRECTORY
wget "https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-cpu-linux-x86_64-1.13.1.tar.gz" -O "tensorflow.tar.gz" 
sudo tar -C /usr/local -xzf "tensorflow.tar.gz" 
sudo ldconfig
rm "tensorflow.tar.gz"

git clone https://github.com/oktal/pistache.git
git submodule update --init
cd pistache
mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make
sudo make install
cd ../../../..


if [ -d "$TEMP_DIRECTORY" ]; then
	rm -rf $TEMP_DIRECTORY;
fi




