#!/bin/bash

source /data/RB230064/common/spiritroot/Submit/SpiRITROOT_WMU/env_Submit.sh

#runs=(1642 1660 1668 1805 1812 1652 1654 1655 1656 1708)
#runs=(1642)
runs=(1149 1165 1283 1292 1296 1305) #spring
#runs=(1149)
#dens=(0 20 40 60 80 100 120 140 160)
dens=("d0000" "d0020" "d0040" "d0060" "d0080" "d0100" "d0120" "d0200" "d0220" "d0240" "d0260" "d0280" "d0300")
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


if [ $mode -eq 0 ]
then
    targetZ=-30.82 #fall
    targetZ_reco=-32.72 #fall
else
    targetZ=-19.48 #spring
    targetZ_reco=-22.31 #spring
fi



inputDir0="/data/RB230064/teajun/SC_test/data/$switch/dens_analysis/" #files were renamed in terminal with bash and don't have .reco.2024 suffix
inputDir1="/data/RB230064/teajun/SC_test/data/$switch/no_SC/"



cut_switch=0
#cut_switch=1
for name in "no_cut" "vtx_cut" "mult_cut"
#for name in "vtx_cut" "mult_cut"
#for name in "no_cut"
do
    outputDir0="/data/RB230064/teajun/SC_test/dens/$switch/reco_at_tgt_lad/ana_at_tgt_lad/${name}/"
    outputDir0r="/data/RB230064/teajun/SC_test/dens/$switch/reco_at_tgt_lad/ana_at_reco_vtx/${name}/"

    mkdir -p $outputDir0
    mkdir -p $outputDir0r

    for run in ${runs[@]}
    do
        mkdir -p $outputDir0/$nodens
        mkdir -p $outputDir0r/$nodens

        root -l -b "SCfinder.cpp($run,$targetZ,$BDCOffsetX,$BDCOffsetY,\"$inputDir1\",\"$nodens\",\"$outputDir0\",$evtNum,$cut_switch)"
        root -l -b "SCfinder.cpp($run,$targetZ_reco,$BDCOffsetX,$BDCOffsetY,\"$inputDir1\",\"$nodens\",\"$outputDir0r\",$evtNum,$cut_switch)"

        for d in ${dens[@]}
        do
            mkdir -p $outputDir0/$d
            mkdir -p $outputDir0r/$d
            
            root -l -b "SCfinder.cpp($run,$targetZ,$BDCOffsetX,$BDCOffsetY,\"$inputDir0\",\"$d\",\"$outputDir0\",$evtNum,$cut_switch)"
            root -l -b "SCfinder.cpp($run,$targetZ_reco,$BDCOffsetX,$BDCOffsetY,\"$inputDir0\",\"$d\",\"$outputDir0r\",$evtNum,$cut_switch)"
        done

    done

    ((cut_switch++))
done
