#!/bin/bash
# Fluka execution script for FEM2Fluka coupling
# V.Boccone April 2011 
# boccone@cern.ch
#
export FLUKA=$FLUKA
export FLUPRO=$FLUPRO

function helpMsg(){
    echo "Script to perform Fluka actions"
    echo " > run.sh -b [bunch id] -x [number of CPUs] -c [cycles] -p [number of primaries] -t [mesh_type]"
    echo " > mesh types : fine, intermediate and coarse"
}

cd $PWD
nCPUs=1
nCycles=5
nPrimaries=200
if [ $# -eq 0 ] ; then
    helpMsg
    exit
fi

set -- `getopt b:x:c:p:t: $*`

if [ $? -ne 0 ] ; then
    helpMsg
    exit 2
fi

while [ $# -gt 0 ] ; do 
    case $1 in
        -b)
            bunchID=${2}
            shift 2
            ;;
        -x) 
            nCPUs=${2}
            shift 2
            ;;
        -c) 
            nCycles=${2}
            shift 2
            ;;
        -p) 
            nPrimaries=${2}
            shift 2
            ;;
        -t) 
            meshType=${2}
            shift 2
            ;;
        --)
            shift
            break
            ;;
    esac
done

echo "bunchID    = $bunchID"
echo "nCPUs      = $nCPUs"
echo "nCycles    = $nCycles"
echo "nPrimaries = $nPrimaries"
echo "meshType   = $meshType"

# Define the PreviousRunVariable
PreviousRun=$(($bunchID-1))

mkdir bunch_${bunchID}_${meshType}
cd bunch_${bunchID}_${meshType}

# if run == 0 initialize a "full tungsten block"
if [ ${bunchID} ==  "0" ] ; then
    ../writeTarget ${bunchID} ${meshType}    
# else take the density map and create the voxel intermediate ascii file
else 
    if [ -f ../results_b${PreviousRun}_${meshType}.txt ]; then 
        ../writeTarget ${bunchID} ../results_b${PreviousRun}_${meshType}.txt ${meshType}
    else 
        echo "ERROR ../results_b$PreviousRun_$meshType.txt  does not exist"
        cd ..
        rm -rf bunch_${bunchID}_${meshType}
        exit
    fi
fi

# quit if can't find the voxel ascii file
if [ -f  target_block.ascii ]; then 
    if [ ${meshType} == "fine" ]; then
	../writeVoxelF
    fi
    if [ ${meshType} == "intermediate" ]; then
	../writeVoxelI
    fi
    if [ ${meshType} == "coarse" ]; then
	../writeVoxelC
    fi
    
else 
    echo "ERROR target_block.ascii does not exist"  
    cd ..
    rm -rf bunch_${bunchID}_${meshType}
    exit 
fi

# expand the input file
../expand.sh ../voxel_target.inp target_b${bunchID}_${meshType}.inp

echo "Running Fluka on $nCPUs CPUs" 
if [ ${nCPUs} == "1" ]; then
        # One CPU case two arguments
    RndNum=`od -An -N3 -i /dev/urandom`
    sed -i "s/SET_RANDOM_NUMBER/$RndNum/g" target_b${bunchID}_${meshType}.inp
    sed -i "s/SET_PARTICLE_NUMBER/$nPrimaries/g" target_b${bunchID}_${meshType}.inp
    $FLUPRO/flutil/rfluka -N0 -M5 target_b${bunchID}_${meshType}
else
        # Split the job on multiple nCPUs 
    for jobs in `seq 1 $nCPUs` ;
      do 
      cp target_b${bunchID}_${meshType}.inp target_b${bunchID}_${meshType}_job${jobs}_.inp 
      cp ../submitJob.sh .
      RndNum=`od -An -N3 -i /dev/urandom`
      echo "$RndNum"
      filename="target_b${bunchID}_${meshType}_job${jobs}_"
      touch lock.${filename}
      sed -i "s/SET_RANDOM_NUMBER/$RndNum/g" ${filename}.inp
      sed -i "s/SET_PARTICLE_NUMBER/$nPrimaries/g" ${filename}.inp          
      submitJob.sh ${filename} $nCycles  > job_${jobs}.log&
    done
    
    
        # Check the locks
    echo " Checking locks"
    locks=0;
    for jobs in `seq 1 $nCPUs`;
      do
      if [ -e lock.target_b${bunchID}_${meshType}_job${jobs}_ ];then
          locks=$(($locks+1))
      fi
    done 
    echo "$locks Lock found, waiting the end of the processes"
    
    while [ $locks -ne "0" ]
      do
      locks=0
      for jobs in `seq 1 $nCPUs`;
        do
        if [ -e lock.target_b${bunchID}_${meshType}_job${jobs}_ ];then
            locks=$(($locks+1))
        fi
          done 
      sleep 10
        done
fi


# Merge and convert the USRBIN to ASCII
ls -1 *.31            >  OutputFileList
echo ""               >> OutputFileList

echo "energy_b${bunchID}_${meshType}"     >> OutputFileList
echo "energy_b${bunchID}_${meshType}"     >  BinToTxt
echo "energy_b${bunchID}_${meshType}.txt" >> BinToTxt

$FLUPRO/flutil/usbsuw < OutputFileList > LogFile
$FLUPRO/flutil/usbrea < BinToTxt       >> LogFile
cp energy_b${bunchID}_${meshType}.txt ../
cd ..
