#!/bin/bash
#runs=(1642 1660 1668 1805 1812 1652 1654 1655 1656 1708)
#runs=(1642)
#runs=(1149 1165 1283 1292 1296 1305) #spring
run=1305

nodens="no_sc"


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
    inputDir="/home/teajun/dens/$switch/reco_at_tgt_lad/test2/${name}/"

    outputDir="/home/teajun/result/$switch/single_run"
    mkdir -p $outputDir
    outputFile="$outputDir/${name}_$run.root"

    root -l -b -q "calc_sc_single.cpp($run,\"$inputDir\",\"$outputFile\")"

done