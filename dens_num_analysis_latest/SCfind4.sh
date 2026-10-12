#!/bin/bash

source /data/RB230064/common/spiritroot/Submit/SpiRITROOT_WMU/env_Submit.sh 

#runs=(1642 1660 1668 1805 1812 1652 1654 1655 1656 1708) #fall
#runs=(1642)
runs=(1149 1165 1283 1292 1296 1305) #spring
runs1=(1305 1165) #lower trig rate (~15 to ~20)
#runs2=(1149 1296) #medium trig rate (~22 to ~30)
#runs3=(1292) #high trig rate (~35)
#runs4=(1283) #higher trig rate (~343)
runs2=(1149) 
runs3=(1296)
runs4=(1292)
runs5=(1283)

runs6=(1170 1306) #test replacement for 1305
runs6=(1170)
runs7=(1163) #test replacement for 1149

runs8=(1274) #test high (~40)
runs9=(1161) #test low (~20)
runs10=(1295) #test med (~30)
runs11=(1290) #test high (~35)

runs12=(1148) #tests for change in attenuation (look at runs log)
runs13=(1151)

#dens=(d0000 d0020 d0040 d0060 d0080 d0100 d0200 d0220 d0240 d0260 d0280 d0300 d0400 d0500 d0600 d0700 d1000 d2000)
#dens=(d0900 d1100 d1200 d1300)
dens=(d0950 d1050 d1150 d1250 d1350)
dens1=(d0900 d1100 d1200 d1300) #low
#dens2=(d1800 d1900 d2000 d2100 d2200) #medium
#dens3=(d2400 d2500 d2600 d2700 d2800) #high
#dens4=(d4700 d5000 d5300 d5600 d5900) #higher

dens2=(d2600 d2700 d2800) #medium
dens3=(d1500 d1600 d1700) #medium
dens4=(d2100 d2200 d2300) #high
#dens5=(d2500 d3000 d3500) #higher
dens5=(d3100 d3200 d3300 d3400 d3600) #higher

#dens6=(d0900 d1000 d1100 d1200 d1300)
#dens7=(d1300 d1400 d1500 d1600 d1700)
dens6=(d0700 d0800)
dens7=(d1100 d1200)

dens8=(d2800 d2900 d3000 d3100 d3200) #test high (~40)
dens9=(d0900 d1000 d1100 d1200 d1300) #test low (~20)
#dens10=(d1600 d1700 d1800 d1900 d2000) #test med (~30)
#dens11=(d2100 d2200 d2300 d2400 d2500) #test high (~35)
dens10=(d2100 d2200) #test med (~30)
dens11=(d2600 d2700) #test high (~35)

dens12=(d2000 d2200 d2400 d2600 d2800) #tests for change in attenuation (look at runs log)
dens13=(d2000 d2500 d3000 d3500 d4000)

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



#for i in 1 2 3 4
for i in 6 7 10 11
do
    declare -n r_i="runs$i" 
    declare -n d_i="dens$i"
    r_i=${r_i[@]}
    d_i=${d_i[@]}
    
    cut_switch=0
    #for name in "no_cut" "vtx_cut" "mult_cut"
    for name in "no_cut"
    do

        outputDir0="/data/RB230064/teajun/SC_test/dens/$switch/test4/dens_analysis/${name}/"

        logDir="/data/RB230064/teajun/slurm_log/$switch/test4/dens_analysis/${name}"
        mkdir -p $logDir


        for run in $r_i
        do
            txtoutput0="$outputDir0/$nodens/"
            mkdir -p $txtoutput0

            #root -b -q -l "SCfinder3.cpp($run, $BDCOffsetX, $BDCOffsetY, \"$inputDir1\", \"$nodens\", \"$outputDir0\", $evtNum, $cut_switch)"
            #sleep 0.3
            for d in $d_i
            do
                txtoutput1="$outputDir0/$d/"
                mkdir -p $txtoutput1
                root -b -q -l "SCfinder3.cpp($run, $BDCOffsetX, $BDCOffsetY, \"$inputDir0\", \"$d\", \"$outputDir0\", $evtNum, $cut_switch)"
            done
        done

        ((cut_switch++))
    done

done