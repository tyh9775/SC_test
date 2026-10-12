#!/bin/bash

source /data/RB230064/common/spiritroot/Submit/SpiRITROOT_WMU/env_Submit.sh 

#runs=(1642 1660 1668 1805 1812 1652 1654 1655 1656 1708) #fall
#runs=(1642)
#runs=(1149 1165 1283 1292 1296 1305) #spring
#runs=(1305 1283)
runs=(1305)
#runs=(1149)
#dens=(d0000 d0020 d0040 d0060 d0080 d0100 d0200 d0220 d0240 d0260 d0280 d0300 d0400 d0500 d0600 d0700 d1000 d2000)
#dens=(d0000)
dens=(d0900 d1100 d1200 d1300)
nodens="no_sc"
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


evtNum=100


#no target z pos needs to be set, just take the reco (or VA) vtx z pos from the data directly

inputDir0="/data/RB230064/teajun/SC_test/data/$switch/dens_analysis"
inputDir1="/data/RB230064/teajun/SC_test/data/$switch/no_SC"



cut_switch=0
#cut_switch=1
#for name in "no_cut" "vtx_cut" "mult_cut"
for name in "no_cut"
do

    outputDir0="/data/RB230064/teajun/SC_test/dens/$switch/test3/dens_analysis/${name}/"

    logDir="/data/RB230064/teajun/slurm_log/$switch/test3/dens_analysis/${name}"
    mkdir -p $logDir


    for run in ${runs[@]}
    do
        txtoutput0="$outputDir0/$nodens/"
        mkdir -p $txtoutput0

        sbatch --partition=mpc --account=RB230064 --output=${logDir}/%x_%j.log sbatch_finder_test.sh $run $BDCOffsetX $BDCOffsetY $inputDir1 $nodens $outputDir0 $evtNum $cut_switch
        sleep 0.3

        for d in ${dens[@]}
        do
            txtoutput1="$outputDir0/$d/"
            mkdir -p $txtoutput1

            sbatch --partition=mpc --account=RB230064 --output=${logDir}/%x_%j.log sbatch_finder_test.sh $run $BDCOffsetX $BDCOffsetY $inputDir0 $d $outputDir0 $evtNum $cut_switch
            sleep 0.3
        done

    done

    ((cut_switch++))
done

