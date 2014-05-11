#!/bin/bash

. ./tests.sh

setupTestDirectory

runTest "common/CmdRead" true
runTest "common/CmdWrite" true
runTest "common/Mux" true
runTest "common/Scalars" true
runTest "common/Types" true
runTest "common/Counter" true
