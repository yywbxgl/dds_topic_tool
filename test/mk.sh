#!/bin/bash

fastddsgen   -replace  -typeros2 -typeobject  HelloWorld.idl 

rm build -rf
mkdir build
cd build
cmake ..
make -j