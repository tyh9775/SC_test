#!/bin/bash

source /data/RB230064/common/spiritroot/Submit/SpiRITROOT_WMU/env_Submit.sh #using environment with new E field magnitude

#runs=(1642 1660 1668 1805 1812 1652 1654 1655 1656 1708) #fall
#runs=(1642)
#runs=(1149 1165 1283 1292 1296 1305) #spring
runs=(1148 1149 1151 1161 1163 1165 1170 1274 1283 1290 1292 1295 1296 1305 1306) #spring

#dens=(0)

mode=1 #0 = fall, 1 =  spring

if [ $mode -eq 0 ]
then
    switch="fall"
elif [ $mode -eq 1 ]
then
    switch="spring"
else
    echo "incorrect mode"
    exit 1
fi


#new BDC Offsets
if [ $mode -eq 0 ]
then
    BDCOffsetX=-2.621 #fall
else
    BDCOffsetX=-4.62 #spring
fi

BDCOffsetY=-180.138

#new B Field offsets
BFieldOffsetX=0.0736
BFieldOffsetY=-20.5502
BFieldOffsetZ=48.9476

#target ladder calculated vertex position 

if [ $mode -eq 0 ]
then
    targetZ=-30.82 #fall
    #targetZ_reco=-32.72 #fall
else
    targetZ=-19.48 #spring
    #targetZ_reco=-22.31 #spring
fi

#sc parameters
if [ $mode -eq 0 ]
then
    p0=1.736e-9
    p1=8.037e-12
else
    p0=-2.016e-8 
    p1=1.995e-9 
fi

file_path_0="/data/RB230064/teajun/SC_test/data/$switch/SC_par/"
mkdir -p $file_path_0

#make log files, change in sbatch.sh
log_dir0="/data/RB230064/teajun/slurm_log/$switch/SC_par/"
mkdir -p $log_dir0


for run in ${runs[@]}
do
    if [ ! -f "/data/RB230064/teajun/RatesSup_sbatch/run${run}.root" ]
    then
        root -l -b "plotTiming_sbatch.cpp($run)"
    fi

    for m in {0..40}
    #for m in {0..0} 
    do
        sbatch --partition=mpc --account=RB230064 sbatch_sc.sh $run $m $1 $file_path_0 $BFieldOffsetX $BFieldOffsetY $BFieldOffsetZ $p0 $p1 $BDCOffsetX $BDCOffsetY $targetZ
        sleep 0.3
    done
done


