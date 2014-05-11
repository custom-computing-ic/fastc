#!/bin/bash

. ./tests.sh

setupTestDirectory

runTest "common/CmdRead" $1
runTest "common/CmdWrite" $1
runTest "common/Mux" $1
runTest "common/Scalars" $1
runTest "common/Types" $1
