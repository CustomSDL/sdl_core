#!/usr/bin/env bash
#
# This script installs build tools, dependencies and
#

# Install build tools
sudo apt-get install git
sudo apt-get install cmake
sudo apt-get install automake autoconf
sudo apt-get install g++

# Install libraries
sudo apt-get install libsqlite3-dev
sudo apt-get install libssl-dev

# Create some links to build of protobuf
ln -s aclocal-1.15 aclocal-1.14
ln -s automake-1.15 automake-1.14

