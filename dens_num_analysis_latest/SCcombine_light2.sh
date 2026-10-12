#!/bin/bash
#runs=(1642 1660 1668 1805 1812 1652 1654 1655 1656 1708)
#runs=(1642)
#runs=(1149 1165 1283 1292 1296 1305) #spring
#runs1=(1305 1165) #lower trig rate (~15 to ~20)
#runs2=(1149 1296) #medium trig rate (~22 to ~30)
#runs3=(1292) #high trig rate (~35)
#runs4=(1283) #higher trig rate (~343)

runs1=(1305 1165)
#runs1a=(1165)
runs2=(1149) 
runs3=(1296)
runs4=(1292)
runs5=(1283)

#runs6=(1170 1306) #test replacement for 1305
runs6=(1170) #test replacement for 1305
runs7=(1163) #test replacement for 1149

runs8=(1274) #test high (~40)
runs9=(1161) #test low (~20)
runs10=(1295) #test med (~30)
runs11=(1290) #test high (~35)

runs12=(1148) #tests for change in attenuation (look at runs log)
runs13=(1151)
runs14=(1306) #test replacement for 1305

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
dens=(\"d0900\" \"d1000\" \"d1100\" \"d1200\" \"d1300\" \"d1800\" \"d1900\" \"d2000\" \"d2100\" \"d2200\" \"d4700\" \"d5000\" \"d5300\" \"d5600\" \"d5900\")
#dens1=(\"d0950\" \"d1050\" \"d1150\" \"d1250\" \"d1350\") #low
#dens2=(\"d1800\" \"d1900\" \"d2000\" \"d2100\" \"d2200\") #medium
#dens3=(\"d2400\" \"d2500\" \"d2600\" \"d2700\" \"d2800\") #high
#dens4=(\"d4700\" \"d5000\" \"d5300\" \"d5600\" \"d5900\") #higher


#dens5=(d2500 d3000 d3500) #higher

dens1=(\"d0950\" \"d1050\" \"d1150\" \"d1250\" \"d1350\") #low
dens2=(\"d1800\" \"d1900\" \"d2000\" \"d2100\" \"d2200\" \"d2600\" \"d2700\" \"d2800\") #medium
dens3=(\"d1500\" \"d1600\" \"d1700\" \"d1800\" \"d1900\" \"d2000\" \"d2100\" \"d2200\") #medium
dens4=(\"d2100\" \"d2200\" \"d2300\" \"d2400\" \"d2500\" \"d2600\" \"d2700\" \"d2800\") #high
#dens5=(\"d1350\" \"d2500\" \"d3000\" \"d3500\" \"d4700\" \"d5000\" \"d5300\" \"d5600\" \"d5900\") #higher
dens5=(\"d3000\" \"d3100\" \"d3200\" \"d3300\" \"d3400\" \"d3500\" \"d3600\") #higher
#dens=("d0000")

dens6=(\"d0700\" \"d0800\" \"d0900\" \"d1000\" \"d1100\" \"d1200\" \"d1300\")
dens7=(\"d1100\" \"d1200\" \"d1300\" \"d1400\" \"d1500\" \"d1600\" \"d1700\")


dens8=(\"d2800\" \"d2900\" \"d3000\" \"d3100\" \"d3200\") #test high (~40)
dens9=(\"d0900\" \"d1000\" \"d1100\" \"d1200\" \"d1300\") #test low (~20)
dens10=(\"d1600\" \"d1700\" \"d1800\" \"d1900\" \"d2000\" \"d2100\" \"d2200\") #test med (~30)
dens11=(\"d2100\" \"d2200\" \"d2300\" \"d2400\" \"d2500\" \"d2600\" \"d2700\") #test high (~35)

dens12=(\"d2000\" \"d2200\" \"d2400\" \"d2600\" \"d2800\") #tests for change in attenuation (look at runs log)
dens13=(\"d2000\" \"d2500\" \"d3000\" \"d3500\" \"d4000\")
dens14=(\"d0900\" \"d1000\" \"d1100\" \"d1200\" \"d1300\")

runs_csv=$(IFS=,; echo "${runs[*]}")
runs1_csv=$(IFS=,; echo "${runs1[*]}")
runs2_csv=$(IFS=,; echo "${runs2[*]}")
runs3_csv=$(IFS=,; echo "${runs3[*]}")
runs4_csv=$(IFS=,; echo "${runs4[*]}")
runs5_csv=$(IFS=,; echo "${runs5[*]}")
runs6_csv=$(IFS=,; echo "${runs6[*]}")
runs7_csv=$(IFS=,; echo "${runs7[*]}")
runs8_csv=$(IFS=,; echo "${runs8[*]}")
runs9_csv=$(IFS=,; echo "${runs9[*]}")
runs10_csv=$(IFS=,; echo "${runs10[*]}")
runs11_csv=$(IFS=,; echo "${runs11[*]}")
runs12_csv=$(IFS=,; echo "${runs12[*]}")
runs13_csv=$(IFS=,; echo "${runs13[*]}")
runs14_csv=$(IFS=,; echo "${runs14[*]}")

dens_csv=$(IFS=,; echo "${dens[*]}")
dens1_csv=$(IFS=,; echo "${dens1[*]}")
dens2_csv=$(IFS=,; echo "${dens2[*]}")
dens3_csv=$(IFS=,; echo "${dens3[*]}")
dens4_csv=$(IFS=,; echo "${dens4[*]}")
dens5_csv=$(IFS=,; echo "${dens5[*]}")
dens6_csv=$(IFS=,; echo "${dens6[*]}")
dens7_csv=$(IFS=,; echo "${dens7[*]}")
dens8_csv=$(IFS=,; echo "${dens8[*]}")
dens9_csv=$(IFS=,; echo "${dens9[*]}")
dens10_csv=$(IFS=,; echo "${dens10[*]}")
dens11_csv=$(IFS=,; echo "${dens11[*]}")
dens12_csv=$(IFS=,; echo "${dens12[*]}")
dens13_csv=$(IFS=,; echo "${dens13[*]}")
dens14_csv=$(IFS=,; echo "${dens14[*]}")

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

list_all=() #everything
dlist_all=()
list_omit=() #without outliers
dlist_omit=()

list_test=()
dlist_test=()
for i in {1..14}
do
    declare -n l="runs${i}_csv"
    list_all+="${list_all:+,}{$l}"
    #echo $l

    declare -n d="dens${i}_csv"
    dlist_all+="${dlist_all:+,}{$d}"


    l_tmp=()

    l_tst=()   
    #echo $l
    declare -n lt="runs$i"
    declare -n dt="dens$i"
    for ri in "${!lt[@]}"
    do
        run=${lt[ri]}
        #echo $run
        #den=${dt[ri]}
        if [ $run == 1305 ] || [ $run == 1306 ]
        then
            continue
        elif [ $run == 1149 ] || [ $run == 1148 ]  || [ $run == 1151 ]
        then
            l_tst+="${l_tst:+,}$run"
        else
            l_tmp+="${l_tmp:+,}$run"
        fi
    done
    if [ ${#l_tmp[@]} -gt 0 ]
    then
        list_omit+="${list_omit:+,}{$l_tmp}"
        dlist_omit+="${dlist_omit:+,}{$d}"
    fi
    
    if [ ${#l_tst[@]} -gt 0 ]
    then
        list_test+="${list_test:+,}{$l_tst}"
        dlist_test+="${dlist_test:+,}{$d}"
    fi

done


#echo $dlist_omit
#echo 
#echo $dlist_all


#for name in "no_cut" "vtx_cut" "mult_cut"
for name in "no_cut"
do

    inputDir1="/home/teajun/dens/$switch/test4/dens_analysis/${name}"

    outputDir1="/home/teajun/result/$switch/test4/"
    mkdir -p $outputDir1
    file_name="${name}_test_all.root"
    file_name2="${name}_test_omt.root"


    otxt=\"p0_all.txt\"
    otxt2=\"p0_omt.txt\"
    #root -l -b -q "SCcombine_light2.cpp({$list_all},{$dlist_all},\"$inputDir1\",\"$outputDir1\",\"$file_name\",$otxt)"
    #root -l -b -q "SCcombine_light2.cpp({$list_omit},{$dlist_omit},\"$inputDir1\",\"$outputDir1\",\"$file_name2\",$otxt2)"


    file_name3="beam_att_change_test.root"
    otxt3=\"p0_ba_test.txt\"
    root -l -b -q "SCcombine_light2.cpp({$list_test},{$dlist_test},\"$inputDir1\",\"$outputDir1\",\"$file_name3\",$otxt2)"

done