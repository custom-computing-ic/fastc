#!/bin/bash

# Setup script for the RTM - ADP demo Run this script to download
# dependencies (Harmonic, Lara and maxcc binaries with required
# includes)

# Check architecture is 64 bit (required for binaries)
function setup {
    echo "Setting up demo. This may take a while..."
    arch=`uname -m`
    if [[ "$arch" == "i686" ]]; then
        echo "Error: 32 bit architecture/OS not supported!"
        exit 1
    fi

    # Check wget is installed
    which wget > /dev/null
    hasWget=$?
    if [[ "$hasWget" != "0" ]]; then
        echo "Error: wget not available!"
        exit 1
    fi

    # Download binaries and headers
    wget -O aspDFE.support.tgz http://goo.gl/MKmgaU && tar xvzf aspDFE.support.tgz
}

function package {
    echo "Packaging demo. Please wait..."
    tar cvzf adpRtmDemo.tar.gz run settings.sh setup.sh src
}

setupFlag=$1
echo $setupFlag

case $setupFlag in
    -s) setup;;
    -p) package;;
esac
