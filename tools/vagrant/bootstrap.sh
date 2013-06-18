#!/bin/bash
# ------------------------------------------------------------------------------
# Vagrant - Virtualized Development 
# Copyright(c) pgRouting Contributors
#
# Virtual environment bootstrap script
# ------------------------------------------------------------------------------

set -e # Exit script immediately on first error.
#set -x # Print commands and their arguments as they are executed.

# Abort provisioning if pgRouting development environment already setup.
# ------------------------------------------------------------------------------
which vim >/dev/null &&
{ echo "osm2pgrouting development environment already setup."; exit 0; }

# Run provisioning
# ------------------------------------------------------------------------------
echo "Installing packages ... this may take some time."
apt-get update -qq
apt-get install -y -qq vim packaging-dev checkinstall libboost-graph-dev libpq-dev libexpat1-dev

