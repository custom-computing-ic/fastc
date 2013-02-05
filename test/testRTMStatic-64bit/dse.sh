#!/bin/bash

PWD=`pwd`
project=`basename $PWD`

mkdir ../exploded

for MemFq in 303
do
    for KFq in 100
    do
        for Par in 1 2 3 6
        do
            for DspFactor in 0 0.5 1
            do
                new_project=${project}_${Par}Pipes_$DspFactor"DSPFactor_"${MemFq}"MHzMem_"${KFq}"MHzStream"
                new_path=../${new_project}
                cp -r . ${new_path}
                params=${new_path}/include/params_dse.h
                echo "#define Par "${Par} > ${params}
                echo "#define DspFactor "${DspFactor} >> ${params}
                echo "#define KFq "${KFq} >> ${params}
                echo "#define MemFq "${MemFq} >> ${params}
                echo "Created new project: "$new_project
                make -C ${new_path} maxc
                mv ${new_path} ../exploded/
            done
        done
    done
done

mv ../exploded .
