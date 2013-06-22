#!/bin/bash

createConf() {
    param=$2
    value=$3
    fixed=$4
    newDesign=$1_${param}${value}
    newDesignDir=designs-$1/$newDesign
    confFile=$newDesignDir/EngineCode/build.conf
    cp $1 $newDesignDir -r
    echo "${param}=${value}" > $confFile
    echo "fixed=${fixed}" >> $confFile
}

echo $1
mkdir -p designs-$1


for par in 2 6 8 12
do
    createConf $1 par ${par} "false"
done

for dspFactor in 0 1 2
do
    createConf $1 dspFactor ${dspFactor} "false"
done

for mantissa in 53 47 41 35
do
    createConf $1 mantissa ${mantissa} "false"
done

for bitsAccuracy in 64 56 48 40
do
    createConf $1 bitsAccuracy ${bitsAccuracy} "true"
done

for maxValue in 50000 100000 500000 20000000
do
    createConf $1 maxValue ${maxValue} "true"
done

#find . -path "*/DFE/Makefile" -exec bash -c 'cd `dirname {}` && make build &' \;

#find . -path "./*_par*/DFE/Makefile" -exec bash -c 'cd `dirname {}` && make build &' \;
