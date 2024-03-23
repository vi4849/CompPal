#!/bin/bash
source /opt/intel/oneapi/setvars.sh &>/dev/null

/bin/echo "##" $(whoami) is running 05_openvkl_vdbvkl
bin/vkl_vdb_tutorial
