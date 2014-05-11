#!/bin/bash

. ./tests.sh

setupTestDirectory

runTest "applications/PassThrough"
runTest "applications/MovingAverage"

# XXX Fix ADP test, support multi kernel test
# runTest "applications/ADP" true
# runTest "applications/RTM"
# runTest "applications/ray1"
