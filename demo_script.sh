#!/bin/bash

echo -e "\033[1;31mUsing platform Virtex 6: BRAMs: 1064, LUTs: 297600, FFs: 595200, DSPs: 2016, Bandwdith: 38400MB/s \033[0m"
echo "Hit any key to continue..."
read ch
./fastc test/testApplications/maxc/RTM.c
echo "Hit any key to continue"
read ch

echo -e "\033[1;31mUsing platform Stratix 5: BRAMs: 2567, LUTs: 695000, FFs: 1050000, DSPs: 1963, Bandwidth: 65000MB/s \033[0m"
cp max4_stratix5 platform.txt
echo "Hit any key to continue..."
read ch
./fastc test/testApplications/maxc/RTM.c


echo "Hit any key to continue"
cp max3_virtex6 platform.txt
