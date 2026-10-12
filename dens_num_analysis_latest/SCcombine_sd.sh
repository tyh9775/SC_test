#!/bin/bash
#runs=(1642 1660 1668 1805 1812 1652 1654 1655 1656 1708)
#runs=(1642)
#runs=(1149 1165 1283 1292 1296 1305) #spring
#runs=(1149 1165 1283 1292) #spring
#runs=(1305 1283)
runs=(1305)
#runs=(1149)
#runs=(1296)

dens=\"single_dens\"
nodens=\"no_sc\"

runs=$(IFS=,; echo "${runs[*]}")

#dens=$(IFS=,; echo "${dens[*]}")

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
    inputDir0="/home/teajun/dens/$switch/test3/dens_analysis/${name}/"
    inputDir1="/home/teajun/dens/$switch/single_dens/${name}/"
    outputDir1="/home/teajun/result/$switch/single_dens"
    mkdir -p $outputDir1
    outputFile0="$outputDir1/no_sc_test.root"
    outputFile1="$outputDir1/${name}_test.root"

    root -l -b -q "SCcombine_sd.cpp({$runs},$dens,\"$inputDir1\",\"$outputFile1\")"
    root -l -b -q "SCcombine_sd.cpp({$runs},$nodens,\"$inputDir0\",\"$outputFile0\")"


done