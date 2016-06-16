#!/usr/bin/env bash
#
# This script installs build tools, dependencies and
# sets up environment to use USB on Ubuntu 14.04
#

# Install build tools
sudo apt-get install git
sudo apt-get install cmake
sudo apt-get install automake autoconf

# Install dependencies of building for Ubuntu 14.04
sudo apt-get install g++
sudo apt-get install libpcap0.8-dev
sudo apt-get install libsqlite3-dev
sudo apt-get install libssl-dev
sudo apt-get install libudev-dev

# Set up environment to use USB on Ubuntu 14.04
echo "SUBSYSTEM==\"usb\", ENV{DEVTYPE}==\"usb_device\", MODE=\"0666\"" | sudo tee /etc/udev/rules.d/usb.rules

