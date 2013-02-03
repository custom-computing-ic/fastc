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




runRemote() {
    cd test/$1 && CLEAN=true MAXC=true bash deploy.sh
}


# --- Local Test Suite ---
runLocalTestSuite() {
    test=$1
    if [ "$test" = "" ]; then
        #   runTest "maxc/testMaxCTemplate"

        #    runTest "maxc/testCmdRead"

        #    runTest "maxc/testCmdWrite"

        #    runTest "maxc/test1dConvolutionKernel"

        #    runTest "maxc/testRTMSingle"

        #   runTest "maxc/testArrays"

        runTest "testCommon/maxc/testCmdRead"

        runTest "testCommon/maxc/testCmdWrite"

        runTest "testTypes/maxc/testTypes"

        runTest "test1dConvolution/maxc/test1dConvolutionKernel"

        #    runTest "testTypes/testCounter"

        #    runTest "maxc/testRTMManager"

    else
        runTest $TEST
    fi
}

# --- Remote Test Suite ---
runRemoteTestSuite() {
    test=$1
    if [ "$test" = "" ]; then

        runRemote "test1dConvolution"

    else
        runRemote $test
    fi
}


############# Test Setup #################

if [ "$REMOTE" = "true" ]; then
    runRemoteTestSuite $TEST
else
    runLocalTestSuite $TEST
fi

############# Test Summary ###############
