#!/bin/sh

projectDir=`pwd`
project=`basename $projectDir`

remoteUser=pg1709
remoteMachine=shell4.doc.ic.ac.uk
buildMachine=cccad1
runMachine=maxstation1

remoteDir='~/maxcc/projects'
remoteProjectDir="${remoteDir}/$project"

if [ "$remoteUser" = "" ]; then
    echo "deploy.sh: 'remoteUser' is not set !!!"
    exit 1
fi

build="Simulation"
if [ "$1" = "hw" ]; then
  build="Hardware"
fi

echo "Deploying project to remote machine"
echo "  project dir        ==> $projectDir"
echo "  build              ==> $build"
echo "  remote user        ==> ${remoteUser}"
echo "  remote machine     ==> ${remoteMachine}"
echo "  build machine      ==> ${buildMachine}"
echo "  run machine        ==> ${runMachine}"
echo "  remote project dir ==> ${remoteProjectDir}"

if [ "$MAXC" = "true" ]; then
	make maxc
fi

scp -r ${projectDir} ${remoteUser}@${remoteMachine}:${remoteDir}

if [ "$build" = "Simulation" ]; then
  echo "Running application (SIM)"
  ssh ${remoteUser}@${remoteMachine} "ssh ${buildMachine} \"make -C ${remoteProjectDir} clean run-sim\""
else
  echo "Building Application (HW)"
  ssh ${remoteUser}@${remoteMachine} "ssh ${buildMachine} \"make -C ${remoteProjectDir} clean build-hw\""

  echo "Running  Application (HW)"
  ssh ${remoteUser}@${remoteMachine} "ssh ${runMachine} \"make -C ${remoteProjectDir} run-hw\""
fi
