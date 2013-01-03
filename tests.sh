#!/bin/sh

############ Configuration ##############

EXE=maxcc


############ Test Functions #############

runTest() {
    printf "Running $1..."
    ${EXE} test/$1.c > test/$1.out
    output=`diff -wbB test/$1.out test/$1.exp`
    res=$?
    if [ "$res" = "1" ]; then
	printf "[Failed!]\n"
	printf " ---- Test output ---- \n"
	printf "$output\n"
	printf " ---------------------\n"
    else 
	printf "[Passed!]\n"
    fi
}


############# Test Setup #################

runTest "maxc/testMaxCTemplate"

############# Test Summary ###############
