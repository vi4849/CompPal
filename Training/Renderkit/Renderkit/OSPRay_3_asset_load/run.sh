#!/bin/bash
source /opt/intel/oneapi/setvars.sh &>/dev/null # activate this for IDC

/bin/echo "##" $(whoami) is running asset_loader
bin/asset_loader $1 $2 $3
