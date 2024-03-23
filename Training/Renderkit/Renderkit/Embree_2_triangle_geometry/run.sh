#!/bin/bash

source /opt/intel/oneapi/setvars.sh > /dev/null 2>&1

/opt/intel/oneapi/embree/latest/bin/embree_triangle_geometry -o test.ppm
#convert-im6 test.ppm test.png
