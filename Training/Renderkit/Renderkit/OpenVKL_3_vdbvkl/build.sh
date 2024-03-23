#!/bin/bash
source /opt/intel/oneapi/setvars.sh

export rkcommon_DIR=/opt/intel/oneapi/rkcommon/latest/lib/cmake/rkcommon
export openvkl_DIR=/opt/inte/oneapi/openvkl/latest/lib/cmake/openvkl

/bin/echo "##" $(whoami) is building 04_openvkl_tutorial

#source /opt/intel/intel/oneapi/setvars.sh

[ ! -d build ] && mkdir -p build
cd build

rm -rf *

cmake .. 

cmake --build . 

