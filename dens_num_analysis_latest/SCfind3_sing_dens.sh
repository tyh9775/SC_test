#!/bin/bash

source /data/RB230064/common/spiritroot/Submit/SpiRITROOT_WMU/env_Submit.sh 

#runs=(1642 1660 1668 1805 1812 1652 1654 1655 1656 1708) #fall
#runs=(1642)
#runs=(1149 1165 1283 1292 1296 1305) #spring
#runs=(1305 1283)
runs=(1305)
#runs=(1149)
dens="single_dens"
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

inputDir0="/data/RB230064/teajun/SC_test/data/$switch/single_dens"


cut_switch=0
#cut_switch=1
#for name in "no_cut" "vtx_cut" "mult_cut"
for name in "no_cut"
do

    outputDir0="/data/RB230064/teajun/SC_test/dens/$switch/single_dens/${name}/"

    logDir="/data/RB230064/teajun/slurm_log/$switch/single_dens/${name}"
    mkdir -p $logDir


    for run in ${runs[@]}
    do
        txtoutput0="$outputDir0/$dens/"
        mkdir -p $txtoutput0

        root -b -q -l "SCfinder3.cpp($run, $BDCOffsetX, $BDCOffsetY, \"$inputDir0\", \"$dens\", \"$outputDir0\", $evtNum, $cut_switch)"
    done

    ((cut_switch++))
done

