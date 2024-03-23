#!/bin/bash
source /opt/intel/oneapi/setvars.sh

unset TBBROOT
unset CMAKE_PREFIX_PATH
unset CPATH

export rkcommon_DIR=/opt/intel/oneapi/rkcommon/latest/lib/cmake/rkcommon-1.5.0
export openvkl_DIR=/opt/intel/oneapi/openvkl/latest/lib/cmake/openvkl-0.11.0

/bin/echo "##" $(whoami) is building 04_openvkl_tutorial

[ ! -d build ] && mkdir -p build
cd build

rm -rf *

cmake .. \
-DCMAKE_MODULE_PATH=/opt/intel/oneapi/rkcommon/latest/lib/cmake/rkcommon-1.5.0

cmake --build . 

