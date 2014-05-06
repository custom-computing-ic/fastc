#!/bin/bash

# Setup script for the RTM - ADP demo Run this script to download
# dependencies (Harmonic, Lara and maxcc binaries with required
# includes)

function setup {
    echo "Setting up demo. This may take a while..."

    # Check architecture is 64 bit (required for binaries)
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

function clean {
    rm -rf *.dot code* *.code *.xml build rose*.c
}

function help {
    echo "Usage: bash setup.sh <flag>"
    echo " <flag> can be:"
    echo "  -s   setup the demo and download dependencies"
    echo "  -r   run the demo"
    echo "  -p   package the demo"
    echo "  -c   clean trash files"
    echo "  -h   print the help message"
}

setupFlag=$1

case $setupFlag in
    -s) setup;;
    -p) package;;
    -c) clean;;
    *)  help;;
esac
