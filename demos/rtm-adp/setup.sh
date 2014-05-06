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

function run {
    source settings.sh

    sources=`find src -name "*.c" | tr "\\n" " "`

    echo "1. Loading ADP - RTM demo"
    echo "  harmonic ==> "`which harmonic`
    echo "  fastc   ==> "`which maxcc`
    echo "  sources  ==> $sources"
    echo "  aspects  ==> $ASP_REP"

    rm -rf designs-RTM
    rm -rf designs-ADP

    #--- RTM Code generation
    # cp src/RTMKernel.c .
    # echo "[RTM] 1. Running Harmonic Weaver"
    # larai $ASP_REP/main.lara -d -a "{csources:'RTMKernel.c', cflags: '-Iinclude/', cparams:[ { Par: 1, Mul: 1}, { Par: 2, Mul: 1 } ], mants:[22, 24]}"
    # echo "[RTM] 2. Compiling FAST ==> MaxJ"
    # find designs/ -name "*.c" -exec fastc -I../../include {} \; -exec bash -c   'dir=`dirname  "{}"` && mv build/engine/*.java "$dir"' \;
    # echo "[RTM] 3. Save and Clean-up"
    # mv designs designs-RTM
    # rm -fr ADPKernel.c RTMKernel.c build rose_*


    cp src/ADPKernel.c .
    # --- ADP Code Generation
    echo "[APD] 1. Running Harmonic weaver"
    #larai $ASP_REP/main.lara -a "{csources:'ADPKernel.c', cflags: '-I../../include/', cparams:[ { N: 8 }, { N: 9}], mants:[10, 11]}"
    larai $ASP_REP/harmonic.lara -x bin/
    cp harmonic.xml $ASP_REP
    larai $ASP_REP/main.lara -a "{csources:'ADPKernel.c', cflags: '-I../../include/', cparams:[ { N: 8 }], mants:[10], aspRoot: '$ASP_REP/'}"
    # echo "[ADP] 2. Compiling FAST ==> MaxJ"
    find designs/ -name "*.c" -exec maxcc -I../../include {} \; -exec bash -c  'dir=`dirname  "{}"` && mv build/engine/*.java "$dir"' \;
    echo "[ADP] 3. Save and Clean-up"
    mv designs designs-ADP
    rm -rf ADPKernel.c RTMKernel.c build rose_*
}

setupFlag=$1

case $setupFlag in
    -s) setup;;
    -p) package;;
    -c) clean;;
    -r) run;;
    *)  help;;
esac
