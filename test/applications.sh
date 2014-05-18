#!/bin/bash

. ./tests.sh

setupTestDirectory

runTest "applications/PassThrough"
runTest "applications/MovingAverage"
runTest "applications/ADP" true
runTest "applications/RTMKernel" true

# XXX Fix ADP test, support multi kernel test
# runTest "applications/RTM"
# runTest "applications/ray1"
