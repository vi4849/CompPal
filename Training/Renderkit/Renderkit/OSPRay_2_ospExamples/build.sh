#!/bin/bash

export rkcommon_DIR=/opt/intel/oneapi/rkcommon/latest/lib/cmake/rkcommon

/bin/echo "##" $(whoami) is compiling ospExamples.cpp
##source /opt/intel/oneapi/setvars.sh
source /opt/intel/oneapi/setvars.sh --force > /dev/null
[ ! -d build ] && mkdir build
cd build
rm -rf *
cmake ..
make -j8

