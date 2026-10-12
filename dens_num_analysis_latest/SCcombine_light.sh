#!/bin/bash
#runs=(1642 1660 1668 1805 1812 1652 1654 1655 1656 1708)
#runs=(1642)
runs=(1149 1165 1283 1292 1296 1305) #spring
#runs=(1149 1165 1283 1292) #spring
#runs=(1305 1283)
#runs=(1305)
#runs=(1149)
#runs=(1296)
#dens=(0 20 40 60 80 100 120 140 160)
#dens=(\"d0000\" \"d0020\" \"d0040\" \"d0060\" \"d0080\" \"d0100\" \"d0120\" \"d0200\" \"d0220\" \"d0240\" \"d0260\" \"d0280\" \"d0300\" \"d0400\" \"d0500\" \"d0600\" \"d0700\" \"d1000\" \"d2000\")
#dens=(\"d0300\" \"d0400\" \"d0500\" \"d0600\" \"d0700\" \"d1000\" \"d2000\")
#dens=(\"d0900\" \"d1000\" \"d1100\" \"d1200\" \"d1300\")
dens=(\"d0950\" \"d1050\" \"d1150\" \"d1250\" \"d1350\")
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

    inputDir1="/home/teajun/dens/$switch/test4/dens_analysis/${name}/"

    outputDir1="/home/teajun/result/$switch/test4/"
    mkdir -p $outputDir1
    file_name="${name}_test2.root"


    root -l -b -q "SCcombine_light.cpp({$runs},{$dens},\"$inputDir1\",\"$outputDir1\",\"$file_name\")"


done