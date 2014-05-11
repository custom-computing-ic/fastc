#!/bin/bash

. ./tests.sh

setupTestDirectory

# XXX Need runMultiKernelTest
# runTest "highlevel/Simple" true
runTest "highlevel/Dfg" true
