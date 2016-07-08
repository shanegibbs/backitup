#!/bin/bash -eu

# TODO add md5sum checks

# reset test dir

rm -rf it
mkdir it
cd it

# create source path for backingup

mkdir files

# start backitup

trap 'kill $(jobs -p)' EXIT
../bin/backitup --interval 1s files &
sleep 1


# Test single file

echo abc > files/bashrc

sleep 2

rm -rf restore
../bin/restoreitup restore '' restore
../../test/compare-dirs.sh files restore


# Delete single file

rm files/bashrc

sleep 2

rm -rf restore
../bin/restoreitup restore '' restore
../../test/compare-dirs.sh files restore


# Test two files

echo abcdef > files/vimrc

sleep 2

rm -rf restore
../bin/restoreitup restore '' restore
../../test/compare-dirs.sh files restore


# Test directory

mkdir files/Documents

sleep 2

rm -rf restore
../bin/restoreitup restore '' restore
../../test/compare-dirs.sh files restore


# Complete

echo IT tests complete
