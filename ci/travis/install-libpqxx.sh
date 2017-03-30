#!/bin/bash
# ------------------------------------------------------------------------------
# Travis CI scripts 
# Copyright(c) pgRouting Contributors
#
# Install osm2pgrouting prerequesits
# ------------------------------------------------------------------------------

set -e


DISTRIBUTION="$1"

if [[ -z "$DISTRIBUTION=" ]] ; then
    exit 1
fi


if [[ "$DISTRIBUTION" = "precise" ]] ; then
    sudo apt-get install -y libpqxx3-dev
elif [[ "$DISTRIBUTION" = "trusty" ]] ; then
    sudo sudo sudo apt-get install -y libpqxx-dev
else 
    exit 1
fi

