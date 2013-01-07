#!/bin/sh

############ Configuration ##############

EXE=maxcc


############ Test Functions #############

runTest() {
    ${EXE} test/$1.c > test/$1.out
    output=`diff -wbB test/$1.out test/$1.exp`
    res=$?
    if [ "$res" = "1" ]; then
	printf "[FAIL!] $1.c\n"
	printf " ---- Diff (meld test/$1.out test/$1.exp) ---- \n"
	printf "$output\n"
	printf " ---------------------\n"
    else
	printf "[OK!]   $1.c\n"
    fi
}


############# Test Setup #################

runTest "maxc/testMaxCTemplate"

runTest "maxc/testCmdRead"

############# Test Summary ###############
