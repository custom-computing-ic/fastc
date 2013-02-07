#!/bin/bash

PWD=`pwd`
project=`basename $PWD`

mkdir ../exploded

for Burst_inc in 1 2 64
do
    for exp in 8
    do
        for mantissa in 24 # 16 18 20 22
        do
            for MemFq in 400 #303
            do
                for KFq in 100
                do
                    for Par in 1 2 3 6
                    do
                        for DspFactor in 1 #0 0.5
                        do
                            new_project=exp_${project}_${Par}P_$DspFactor"DSP_"${MemFq}"Mem_"${KFq}"Str_f"${exp}"_"${mantissa}"_b"${Burst_inc}
                            new_path=../${new_project}
                            cp -r . ${new_path}
                            params=${new_path}/include/params_dse.h
                            echo "#define Par "${Par} > ${params}
                            echo "#define Mul 1" >> ${params}
                            echo "#define DspFactor "${DspFactor} >> ${params}
                            echo "#define KFq "${KFq} >> ${params}
                            echo "#define MemFq "${MemFq} >> ${params}
                            echo "#define realType "${exp}", "${mantissa} >> ${params}
                            echo "#define Burst_inc "${Burst_inc} >> ${params}
                            echo "Created new project: "$new_project
                            make -C ${new_path} maxc
                            mv ${new_path} ../exploded/
                        done
                    done
                done
            done
        done
    done
done
mv ../exploded .
cp ../deploy-inc.sh exploded
