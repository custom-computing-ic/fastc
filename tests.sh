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

# runs a single local test, printing success or fail status and diff
# from expected result. The expected directory structure is:
# testDirectory/TestKernel.c -- the file to test
# testDirectory/TestKernel.exp -- the MaxJ design to compare the result against
# usage: runTest <testDirectory>
runTest() {
    # make sure we're in root directory
    cd $MY_PATH
    base=`basename $1`
    dir=`dirname $1`

    # change to test directory
    cd test/$dir && $EXE $INC $base.c
    runOut=build/engine/$base.java
    echo $runOut
    output=`diff -wbB $runOut $base.exp`
    res=$?
    if [ "$res" = "0" ]; then
        printf "\033[0;32m[OK!]\033[0m   $1.c\n"
    else
        printf "\033[0;31m[FAIL!]\033[0m $1.c\n"
        printf " ---- Diff (meld test/$dir/$runOut test/$dir/$base.exp) ---- \n"
        printf "$output\n"
        printf " ---------------------\n"

        # start meld if available
        difftool=`which meld`
        if [ "$difftool" != "" ]; then
            meld $MY_PATH/test/$dir/$runOut $MY_PATH/test/$dir/$base.exp &
        fi
    fi
}


runRemote() {
    cd test/$1 && CLEAN=true MAXC=true bash deploy.sh
}


# --- Local Test Suite ---

# Runs the local test suite.
#
# usage: runLocalTestSuite [testName]
#   The optional testName argument can be passed to run a single test.
#   The test name correspond to the test directory structure.
runLocalTestSuite() {
    test=$1
    if [ "$test" = "" ]; then

        runTest "testDFG/maxc/DFG"

        runTest "testRTMStatic/maxc/RTM"

        runTest "testArrays/maxc/Arrays"

        runTest "testCommon/maxc/CmdRead"

        runTest "testCommon/maxc/CmdWrite"

        runTest "testTypes/maxc/Types"

        runTest "test1dConvolution/maxc/Convolution1d"

        runTest "testPassThrough/maxc/PassThroughKernel"

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
