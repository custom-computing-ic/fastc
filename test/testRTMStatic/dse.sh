#!/bin/bash

PWD=`pwd`
project=`basename $PWD`

mkdir ../exploded

for dim_y in 32 128 256 512
do
    for Burst_inc in 64 #1 2 64
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
                                new_project=exp_${project}_${Par}P_$DspFactor"DSP_"${MemFq}"Mem_"${KFq}"Str_f"${exp}"_"${mantissa}"_b"${Burst_inc}"_ny"${dim_y}
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

                                if [ "$dim_y" = "32" ]; then
                                    min_nx=24
                                    max_nx=48
                                elif  [ "$dim_y" = "128" ]; then
                                    min_nx=128
                                    max_nx=256
                                elif [ "$dim_y" = "256" ]; then
                                    min_nx=128
                                    max_nx=256
                                else
                                    min_nx=256
                                    max_nx=512
                                fi

                                echo "#define min_nx "${min_nx} >> ${params}
                                echo "#define max_nx "${max_nx} >> ${params}
                                echo "#define dim_y "${dim_y} >> ${params}

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
done

mv ../exploded .
cp ../deploy-inc.sh exploded
