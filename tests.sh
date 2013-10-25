#!/bin/sh

############ Configuration ##############

# path of this script
MY_PATH=`pwd`

# the fastc executable to use
EXE=$MY_PATH/fastc

# compilation includes
INC=-I/$MY_PATH/include

# the directory in which tests are run. This directory is cleaned
# before each test run to ensure
TEST_DIR=test_run


############ Test Functions #############

runTest() {
    base=`basename $1`
    dir=`dirname $1`
    cd test/$dir && $EXE $INC $base.c
    echo $base
    echo $dir
    runOut=build/engine/$base.java
    echo $runOut
    output=`diff -wbB $runOut $base.exp`
    res=$?
    if [ "$res" = "1" ]; then
        printf "[FAIL!] $1.c\n"
        printf " ---- Diff (meld test/$dir/$runOut test/$dir/$base.exp) ---- \n"
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

        runTest "testRTMStatic/maxc/RTM"

#        runTest "testArrays/maxc/Arrays"

#        runTest "testCommon/maxc/testCmdRead"

#        runTest "testCommon/maxc/testCmdWrite"

#        runTest "testTypes/maxc/testTypes"

#        runTest "test1dConvolution/maxc/test1dConvolutionKernel"

#        runTest "testPassThrough/maxc/testPassThroughKernel"

#        runTest "testCounter/maxc/testCounter"

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
