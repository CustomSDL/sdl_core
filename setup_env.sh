#!/usr/bin/env bash
#
# This script installs build tools, dependencies and
#

# Install build tools
sudo apt-get install cmake
sudo apt-get install automake autoconf
sudo apt-get install g++
sudo apt-get install pkgconf
sudo apt-get install libtool

# Install libraries
sudo apt-get install libsqlite3-dev
sudo apt-get install libssl-dev
