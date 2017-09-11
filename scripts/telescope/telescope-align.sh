#!/bin/sh -ex
#
# run telescope tracking

RUN=$1; shift
FLAGS=$@ # e.g. -n 10000, to process only the first 10k events

#rawfile=$(printf "../../raw/cosmic_%06d_000000.root" $RUN)
rawfile=$(printf "/eos/atlas/atlascerngroupdisk/pixel-upgrade/cmos/TowerJazz/Investigator/Testbeam/Testbeam2017/converted/cosmic_%06d_000000.root" $RUN)
prefix=$(printf "../../output/cosmic_%06d_000000-" $RUN)

echo "using $(which pt-align)"

pt-align $FLAGS \
    -d ../../config/device.toml \
    -c ../../config/analysis.toml \
    -g ../../config/geometry.toml \
    -u tel_coarse \
    ${rawfile} ${prefix}align_coarse

pt-align $FLAGS \
    -d ../../config/device.toml \
    -c ../../config/analysis.toml \
    -g ${prefix}align_coarse-geo.toml \
    -u tel_fine \
    ${rawfile} ${prefix}align_fine

echo 