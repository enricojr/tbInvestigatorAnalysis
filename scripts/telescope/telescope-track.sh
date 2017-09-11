#!/bin/sh -ex

RUN=$1
RUNALIGN=$2

rawfile=$(printf "/eos/atlas/atlascerngroupdisk/pixel-upgrade/cmos/TowerJazz/Investigator/Testbeam/Testbeam2017/converted/cosmic_%06d_000000.root" $RUN)
prefix=$(printf "../../output/cosmic_%06d_000000-" $RUN)
prefixAlign=$(printf "/eos/atlas/atlascerngroupdisk/pixel-upgrade/cmos/TowerJazz/Investigator/Testbeam/Testbeam2017/converted/cosmic_%06d_000000-" $RUNALIGN)

echo "using $(which pt-track)"

pt-track \
  -d ../../config/device.toml \
  -c ../../config/analysis.toml \
  -g ${prefixAlign}align_fine-geo.toml \
  ${rawfile} ${prefix}track
