#!/bin/bash

. ./tests.sh

setupTestDirectory

# XXX Need runMultiKernelTest
# runTest "highlevel/Simple" true
runTest "highlevel/Dfg" true
runTest "highlevel/ray1" true
runTest "highlevel/ray2" true
