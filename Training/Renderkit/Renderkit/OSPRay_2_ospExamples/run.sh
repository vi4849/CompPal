#!/bin/bash
source /opt/intel/oneapi/embree/latest/env/vars.sh
source /opt/intel/oneapi/openvkl/latest/env/vars.sh
source /opt/intel/oneapi/rkcommon/latest/env/vars.sh
source /opt/intel/oneapi/ospray/latest/env/vars.sh

/bin/echo "##" $(whoami) is running ospExamples
bin/ospExamples $1 $2
