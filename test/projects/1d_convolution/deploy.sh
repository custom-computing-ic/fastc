#!/bin/sh

projectDir=`pwd`
project=`basename $projectDir`

remoteMachine=pg1709@shell4.doc.ic.ac.uk
buildMachine=cccad1
runMachine=maxstation1

remoteDir='~/maxcc/projects'
remoteProjectDir="${remoteDir}/$project"


build="Simulation"
if [ "$1" = "hw" ]; then
  build="Hardware"
fi

echo "Deploying project to remote machine"
echo "  project dir        ==> $projectDir"
echo "  build              ==> $build"
echo "  remote machine     ==> ${remoteMachine}"
echo "  build machine      ==> ${buildMachine}"
echo "  run machine        ==> ${runMachine}"
echo "  remote project dir ==> ${remoteProjectDir}"

make maxc

scp -r ${projectDir} ${remoteMachine}:${remoteDir}

if [ "$build" = "Simulation" ]; then
  echo "Running application (SIM)"
  ssh ${remoteMachine} "ssh ${buildMachine} \"make -C ${remoteProjectDir} clean run-sim\""
else
  echo "Building Application (HW)"
  ssh ${remoteMachine} "ssh ${buildMachine} \"make -C ${remoteProjectDir} clean build-hw\""

  echo "Running  Application (HW)"
  ssh ${remoteMachine} "ssh ${runMachine} \"make -C ${remoteProjectDir} run-hw\""
fi
