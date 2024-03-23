#!/bin/bash
source /opt/intel/oneapi/setvars.sh
rm -rf build/
[ ! -d build ] && mkdir build
cd build
cmake ..
cmake --build .

