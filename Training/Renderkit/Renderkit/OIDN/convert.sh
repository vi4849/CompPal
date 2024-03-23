#!/bin/bash
file=$1
base=${file%%.hdr.pfm}
convert-im6 $base.hdr.pfm -colorspace RGB -colorspace sRGB -auto-gamma $base.hdr.png
convert-im6 $base.out.pfm -colorspace RGB -colorspace sRGB -auto-gamma $base.out.png
