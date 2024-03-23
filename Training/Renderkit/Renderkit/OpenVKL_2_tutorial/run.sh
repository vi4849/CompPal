#!/bin/bash
source /opt/intel/oneapi/setvars.sh &>/dev/null

/bin/echo "##" $(whoami) is running 04_openvkl_tutorial
build/vklTutorial
