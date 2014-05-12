#!/bin/bash

. ./tests.sh

setupTestDirectory

# Tests for features in progress
# Expected to fail
runTest "highlevel/ray1" true
runTest "highlevel/ray2" true
