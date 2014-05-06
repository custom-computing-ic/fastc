#!/bin/bash

#binaries (harmonic,larai, fastc)
export PATH=$(readlink -m ../bin/):${PATH}

#aspect repository
export ASP_REP=$(readlink -m aspects/)
