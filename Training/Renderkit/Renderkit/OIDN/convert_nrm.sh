#!/bin/bash
file=$1
base=${file%%.nrm.pfm}
convert-im6 $base.nrm.pfm -colorspace RGB -colorspace sRGB -auto-gamma $base.nrm.png
convert-im6 $base.out.pfm -colorspace RGB -colorspace sRGB -auto-gamma $base.out.png
