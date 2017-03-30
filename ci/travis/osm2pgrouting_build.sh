#!/bin/bash
# ------------------------------------------------------------------------------
# Travis CI scripts 
# Copyright(c) pgRouting Contributors
#
# Build pgRouting
# ------------------------------------------------------------------------------

# exit script on error
set -e 

# build osm2pgrouting
mkdir build
cd build
cmake .. 
make
sudo make install
cd ..
