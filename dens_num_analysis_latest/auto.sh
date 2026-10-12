#!/bin/bash

source /data/RB230064/common/spiritroot/Submit/SpiRITROOT_WMU/env_Submit.sh #using environment with new E field magnitude

#runs=(1642 1660 1668 1805 1812 1652 1654 1655 1656 1708) #fall
#runs=(1642)
runs=(1149 1165 1283 1292 1296 1305) #spring
#runs=(1305 1283)
#runs=(1305)
#runs=(1149)
#dens=(0 20 40 60 80 100 120 140 160)
#dens=(0 20 40 60 80 100 120)
#dens=(200 220 240 260 280 300)
#dens=(400 500 600 700 1000 2000)
#dens=(900 1100 1200 1300)
dens=(950 1050 1150 1250 1350)

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

file_path_0="/data/RB230064/teajun/SC_test/data/$switch/dens_analysis/"
file_path_1="/data/RB230064/teajun/SC_test/data/$switch/no_SC/"


mkdir -p $file_path_0
mkdir -p $file_path_1

#make log files, change in sbatch.sh
log_dir0="/data/RB230064/teajun/slurm_log/$switch/dens_analysis/"
log_dir1="/data/RB230064/teajun/slurm_log/$switch/no_sc/" 
mkdir -p $log_dir0
mkdir -p $log_dir1


for run in ${runs[@]}
do
    if [ ! -f "/data/RB230064/teajun/RatesSup_sbatch/run${run}.root" ]
    then
        root -l -b "plotTiming_sbatch.cpp($run)"
    fi
    for m in {0..40}
    #for m in {0..0} 
    do
        #sbatch --partition=mpc --account=RB230064 sbatch_no_sc.sh $run $m $1 $file_path_1 $BDCOffsetX $BDCOffsetY $targetZ
        #sleep 0.3
        
        for d in ${dens[@]}
        do
            sbatch --partition=mpc --account=RB230064 sbatch_dens.sh $run $m $1 $file_path_0 $BFieldOffsetX $BFieldOffsetY $BFieldOffsetZ $d $BDCOffsetX $BDCOffsetY $targetZ
            sleep 0.3
        done
    done
done


