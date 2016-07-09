#!/bin/bash -eu

# compares timestamps and md5 sum of dirs
compare_dirs() {
  from="$1"
  to="$2"

  bash -c "cd $from; ls -lTR ." > /tmp/a
  bash -c "cd $from; find . -type f -exec md5 {} \; |sort" >> /tmp/a

  bash -c "cd $to; ls -lTR ." > /tmp/b
  bash -c "cd $to; find . -type f -exec md5 {} \; |sort" >> /tmp/b

  diff -u /tmp/a /tmp/b
}

# restores latest backup
test_restore() {
  sleep 2
  rm -rf restore
  ../bin/restoreitup restore '' restore
  compare_dirs files restore
  echo "Tested restore OK"
}

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
test_restore


# Delete single file

rm files/bashrc
test_restore


# Test two files

echo abcdef > files/vimrc
test_restore


# Test directory

mkdir files/Documents
test_restore


# Test files in directory

echo 123 > files/Documents/1.doc
echo 456 > files/Documents/2.doc
echo 789 > files/Documents/3.doc
test_restore


# Test delete directory

rm -rf files/Documents
test_restore


# Complete

echo IT tests complete
