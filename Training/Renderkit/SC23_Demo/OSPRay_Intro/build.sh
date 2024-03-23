#!/bin/bash
## source /opt/intel/inteloneapi/setvars.sh
source /opt/intel/oneapi/setvars.sh --force

export rkcommon_DIR=/opt/intel/oneapi/rkcommon/latest/lib/cmake/rkcommon

/bin/echo "##" $(whoami) is compiling O2_OSPRay_Intro
[ ! -d build ] && mkdir build
cd build
rm -rf *
cmake ..
make -j12 $1
