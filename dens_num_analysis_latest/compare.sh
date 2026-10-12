#!/bin/bash

run=1305

#format d0000, d0020, etc
dens=d0300

#for name in "no_cut" "vtx_cut" "mult_cut"
for name in "no_cut" 
do
    #no sc
    inputFile0="/home/teajun/dens/spring/reco_at_tgt_lad/test2/${name}/density${run}_no_sc.root"

    #some density
    inputFile1="/home/teajun/dens/spring/reco_at_tgt_lad/test2/${name}/density${run}_${dens}.root"


    outputDir="/home/teajun/result/spring/quick_comp"
    mkdir -p $outputDir

    outputFile="$outputDir/${name}_${run}_${dens}.root"

    root -l -b -q "compare.cpp($run,\"$inputFile0\",\"$inputFile1\",\"$outputFile\")"

done