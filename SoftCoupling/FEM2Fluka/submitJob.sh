#!/bin/bash
export FLUKA=$FLUKA
export FLUPRO=$FLUPRO
filename=$1
cycles=$2
$FLUPRO/flutil/rfluka -N0 -M${cycles} ${filename}
rm lock.${filename}
