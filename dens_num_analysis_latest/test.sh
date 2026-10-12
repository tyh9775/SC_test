#!/bin/bash

source /data/RB230064/common/spiritroot/Submit/SpiRITROOT_WMU/env_Submit.sh #using environment with new E field magnitude

#runs=(1642 1660 1668 1805 1812 1652 1654 1655 1656 1708) #fall
#runs=(1642)
runs=(1149 1165 1283 1292 1296 1305) #spring
runs1=(1305 1165) #lower trig rate (~15 to ~20)
runs2=(1149 1296) #medium trig rate (~22 to ~30)
runs3=(1292) #high trig rate (~35)
runs4=(1283) #higher trig rate (~343)
#runs=(1305 1283)
#runs=(1305)
#runs=(1149)
#dens=(0 20 40 60 80 100 120 140 160)
#dens=(0 20 40 60 80 100 120)
#dens=(200 220 240 260 280 300)
#dens=(400 500 600 700 1000 2000)
#dens=(900 1100 1200 1300)
dens=(950 1050 1150 1250 1350)
dens1=(900 1100 1200 1300) #low
dens2=(1800 1900 2000 2100 2200) #medium
dens3=(2400 2500 2600 2700 2800) #high
dens4=(4700 5000 5300 5600 5900) #higher
#dens=(0)

runs_all=($runs1 $runs2 $runs1 $runs4)

dens_all=($dens1 $dens2 $dens3 $dens4)

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

#new B Field offsets
BFieldOffsetX=0.0736
BFieldOffsetY=-20.5502
BFieldOffsetZ=48.9476

#target ladder calculated vertex position 

if [ $mode -eq 0 ]
then
    targetZ=-30.82 #fall
    #targetZ_reco=-32.72 #fall
else
    targetZ=-19.48 #spring
    #targetZ_reco=-22.31 #spring
fi



#make log files, change in sbatch.sh

for i in 1 2 3 4
do
    declare -n r_i="runs$i" 
    declare -n d_i="dens$i"
    r_i=${r_i[@]}
    d_i=${d_i[@]}
    for r in $r_i
    do
        echo $r
        for d in $d_i
        do
            echo $d
        done
    done
done

