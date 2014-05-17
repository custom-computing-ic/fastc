#!/bin/bash

. ./tests.sh

setupTestDirectory

runTest "applications/PassThrough"
runTest "applications/MovingAverage"
runTest "applications/ADP" true

# XXX Fix ADP test, support multi kernel test
# runTest "applications/RTM"
# runTest "applications/ray1"
