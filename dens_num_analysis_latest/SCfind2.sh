#!/bin/bash

source /data/RB230064/common/spiritroot/Submit/SpiRITROOT_WMU/env_Submit.sh

#runs=(1642 1660 1668 1805 1812 1652 1654 1655 1656 1708)
#runs=(1642)
#runs=(1149 1165 1283 1292 1296 1305) #spring
#runs=(1149)
runs=(1305 1283)
#runs=(1149 1165 1292 1296)

#dens=("d0000" "d0020" "d0040" "d0060" "d0080" "d0100" "d0120" "d0200" "d0220" "d0240" "d0260" "d0280" "d0300")
#dens=("d0000" "d0020" "d0040" "d0060" "d0080" "d0100" "d0200" "d0220" "d0240" "d0260" "d0280" "d0300")
dens=("d0400" "d0500" "d0600" "d0700" "d1000" "d2000")
#dens=("d0000")

nodens=no_sc

evtNum=100


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



inputDir0="/data/RB230064/teajun/SC_test/data/$switch/dens_analysis/" #files were renamed in terminal with bash and don't have .reco.2024 suffix
inputDir1="/data/RB230064/teajun/SC_test/data/$switch/no_SC/"



cut_switch=0
#cut_switch=2
#for name in "no_cut" "vtx_cut" "mult_cut"
#for name in "mult_cut"
for name in "no_cut"
do
    outputDir0="/data/RB230064/teajun/SC_test/dens/$switch/reco_at_tgt_lad/test2/${name}/"

    mkdir -p $outputDir0

    for run in ${runs[@]}
    do
        mkdir -p $outputDir0/$nodens

        root -l -b "SCfinder2.cpp($run,$BDCOffsetX,$BDCOffsetY,\"$inputDir1\",\"$nodens\",\"$outputDir0\",$evtNum,$cut_switch)"
        for d in ${dens[@]}
        do
            mkdir -p $outputDir0/$d
            
            root -l -b "SCfinder2.cpp($run,$BDCOffsetX,$BDCOffsetY,\"$inputDir0\",\"$d\",\"$outputDir0\",$evtNum,$cut_switch)"
        done

    done

    ((cut_switch++))
done
