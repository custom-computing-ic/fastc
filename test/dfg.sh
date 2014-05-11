#!/bin/bash

. ./tests.sh

setupTestDirectory
runTest "testDFG/maxc/DFG" true
