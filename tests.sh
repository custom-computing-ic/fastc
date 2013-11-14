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
TEST_RUN_DIR=test_run

############ Test Functions #############

# runs an existing external diff tool to compare the given files
runDiffTool() {
    file1=$1
    file2=$2

    # run meld if available
    difftool=`which meld > /dev/null 2>/dev/null`
    res=$?
    if [ "$res" = "0" ]; then
        meld $file1 $file2
        return
    fi

    diff $file1 $file2
}

print_info() {
    printf "\033[0;33m$1\033[0m"
}

print_error() {
    printf "\033[0;31m$1\033[0m"
}

showOutput() {
    test_log_file=$1
    print_info "Test Output ($test_log_file)\n"
    cat $test_log_file
    printf "\n"
}

# runs a single local test, printing success or fail status and diff
# from expected result. The expected directory structure is:
# testDirectory/TestKernel.c -- the file to test
# testDirectory/TestKernel.exp -- the MaxJ design to compare the result against
#
# usage: runTest <testDirectory> [output]
#   set output=true to display test output on the console; any other values,
#   the test run is logged and only printed in the case of a failing test
runTest() {

    # make sure we're in root directory
    cd $MY_PATH
    base=`basename $1`
    dir=`dirname $1`
    show_output=$2

    # absolute path to test directory
    test_abs_path=$MY_PATH/$TEST_RUN_DIR/$dir
    test_rel_path=$TEST_RUN_DIR/$dir

    # change to test directory
    test_log_file=$test_abs_path/test.log
    cd $test_abs_path

    # run the test
    $EXE $INC $base.c > $test_log_file 2>$test_log_file

    runOut=build/engine/$base.java
    output=`diff -wbB $runOut $base.exp >/dev/null 2>/dev/null`
    res=$?
    if [ "$res" = "0" ]; then
        printf "\033[0;32m[OK!]\033[0m   $1.c\n\n"
        if [ "$show_output" = "true" ]; then
            showOutput $test_log_file
        fi
    else
        print_error "[FAIL!]"
        printf " $1.c\n"
        print_info "Diff (meld $test_rel_path/$runOut $test_rel_path/$base.exp)\n"
        runDiffTool $test_abs_path/$runOut $test_abs_path/$base.exp

        showOutput $test_log_file
    fi
}


runRemote() {
    cd test/$1 && CLEAN=true MAXC=true bash deploy.sh
}


# --- Local Test Suite ---

# Runs the local test suite.
#
# usage: runLocalTestSuite
runLocalTestSuite() {
    runTest "testDFG/maxc/DFG" $1
    runTest "testRTMStatic/maxc/RTM" $1
    runTest "testArrays/maxc/Arrays" $1
    runTest "testCommon/maxc/CmdRead" $1
    runTest "testCommon/maxc/CmdWrite" $1
    runTest "testCommon/maxc/Mux" $1
    runTest "testCommon/maxc/Scalars" $1
    runTest "testApplications/maxc/MovingAverage" $1
    runTest "testTypes/maxc/Types" $1
    runTest "test1dConvolution/maxc/Convolution1d" $1
    runTest "testPassThrough/maxc/PassThroughKernel" $1
    runTest "testCounter/maxc/Counter" $1
    runTest "testFASTCIntegrated/maxc/main" $1
    #    runTest "maxc/testRTMManager"
}

# runs a single test.
#
# usage: runLocalTest <TestName> [ShowOutput]
runLocalTest() {
    test=$1
    output=$2
    runTest $test $output
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

setupTestDirectory() {
    # cleanup test dir and move all tests to the testdir
    rm -rf $TEST_RUN_DIR
    cp -r test $TEST_RUN_DIR
}


############# Test Setup #################

if [ "$REMOTE" = "true" ]; then
    runRemoteTestSuite $TEST
else
    setupTestDirectory
    if [ "$TEST" = "" ]; then
        runLocalTestSuite $OUTPUT
    else
        runLocalTest $TEST $OUTPUT
    fi
fi

############# Test Summary ###############
