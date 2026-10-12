#!/bin/bash
#runs=(1642 1660 1668 1805 1812 1652 1654 1655 1656 1708)
#runs=(1642)
#runs=(1149 1165 1283 1292 1296 1305) #spring
runs=(1149 1165 1283 1292) #spring
#runs=(1149)
#runs=(1296)
#dens=(0 20 40 60 80 100 120 140 160)
dens=(\"d0000\" \"d0020\" \"d0040\" \"d0060\" \"d0080\" \"d0100\" \"d0120\" \"d0200\" \"d0220\" \"d0240\" \"d0260\" \"d0280\" \"d0300\")
#dens=("d0000")

runs=$(IFS=,; echo "${runs[*]}")

dens=$(IFS=,; echo "${dens[*]}")

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




#for name in "no_cut" "vtx_cut" "mult_cut"
for name in "no_cut"
do
    #inputDir0="/home/teajun/dens/$switch/reco_at_tgt_lad/ana_at_tgt_lad/${name}/"
    inputDir0r="/home/teajun/dens/$switch/reco_at_tgt_lad/ana_at_reco_vtx/${name}/"

    #outputFile0="/home/teajun/result/$switch/reco_tgt_ana_tgt_${name}.root"
    outputFile0r="/home/teajun/result/$switch/reco_tgt_ana_reco_${name}.root"

    #root -l -b -q "SCcombine.cpp({$runs},{$dens},\"$inputDir0\",\"$outputFile0\")"
    root -l -b -q "SCcombine.cpp({$runs},{$dens},\"$inputDir0r\",\"$outputFile0r\")"

done