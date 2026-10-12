#!/bin/bash

source /data/RB230064/common/spiritroot/Submit/SpiRITROOT_WMU/env_Submit.sh #using environment with new E field magnitude

#runs=(1642 1660 1668 1805 1812 1652 1654 1655 1656 1708) #fall
#runs=(1642)
runs=(1149 1165 1283 1292 1296 1305) #spring
runs1=(1305 1165) #lower trig rate (~15 to ~20)
#runs2=(1149 1296) #medium trig rate (~22 to ~30)
#runs3=(1292) #high trig rate (~35)
#runs4=(1283) #higher trig rate (~43)

runs2=(1149) 
runs3=(1296)
runs4=(1292)
runs5=(1283)

#runs6=(1170 1306) #test replacement for 1305
runs6=(1170)
runs7=(1163) #test replacement for 1149

runs8=(1274) #test high (~40)
runs9=(1161) #test low (~20)
runs10=(1295) #test med (~30)
runs11=(1290) #test high (~35)

runs12=(1148) #tests for change in attenuation (look at runs log)
runs13=(1151)


#dens=(0 20 40 60 80 100 120 140 160)
#dens=(0 20 40 60 80 100 120)
#dens=(200 220 240 260 280 300)
#dens=(400 500 600 700 1000 2000)
#dens=(900 1100 1200 1300)
#dens=(950 1050 1150 1250 1350)
dens1=(900 1100 1200 1300) #low
#dens2=(1800 1900 2000 2100 2200) #medium
#dens3=(2400 2500 2600 2700 2800) #high
#dens4=(4700 5000 5300 5600 5900) #higher

dens2=(2600 2700 2800) #medium
dens3=(1500 1600 1700) #medium
dens4=(2100 2200 2300) #high
#dens5=(2500 3000 3500) #higher
dens5=(3100 3200 3300 3400 3600) #higher

#dens6=(900 1000 1100 1200 1300)
dens6=(700 800)
#dens7=(1300 1400 1500 1600 1700)
dens7=(1100 1200)

dens8=(2800 2900 3000 3100 3200) #test high (~40)
dens9=(900 1000 1100 1200 1300) #test low (~20)
#dens10=(1600 1700 1800 1900 2000) #test med (~30)
dens10=(2100 2200) #test med (~30)
#dens11=(2100 2200 2300 2400 2500) #test high (~35)
dens11=(2600 2700) #test high (~35)

dens12=(2000 2200 2400 2600 2800) #tests for change in attenuation (look at runs log)
dens13=(2000 2500 3000 3500 4000)

#dens=(0)

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

file_path_0="/data/RB230064/teajun/SC_test/data/$switch/dens_analysis/"
file_path_1="/data/RB230064/teajun/SC_test/data/$switch/no_SC/"


mkdir -p $file_path_0
mkdir -p $file_path_1

#make log files, change in sbatch.sh
log_dir0="/data/RB230064/teajun/slurm_log/$switch/dens_analysis/"
log_dir1="/data/RB230064/teajun/slurm_log/$switch/no_sc/" 
mkdir -p $log_dir0
mkdir -p $log_dir1


#for i in 1 2 3 4
for i in 6 7 10 11
do
    declare -n r_i="runs$i" 
    declare -n d_i="dens$i"
    r_i=${r_i[@]}
    d_i=${d_i[@]}

    for run in $r_i
    do
        if [ ! -f "/data/RB230064/teajun/RatesSup_sbatch/run${run}.root" ]
        then
            root -l -b "plotTiming_sbatch.cpp($run)"
        fi
        #echo $run
        for m in {0..40}
        #for m in {0..0} 
        do
            #sbatch --partition=mpc --account=RB230064 sbatch_no_sc.sh $run $m $1 $file_path_1 $BDCOffsetX $BDCOffsetY $targetZ
            #sleep 0.3
            
            for d in $d_i
            do
                #echo $d
                sbatch --partition=mpc --account=RB230064 sbatch_dens.sh $run $m $1 $file_path_0 $BFieldOffsetX $BFieldOffsetY $BFieldOffsetZ $d $BDCOffsetX $BDCOffsetY $targetZ
                sleep 0.3
            done
        done
    done
done

