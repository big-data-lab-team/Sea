#!/bin/bash

#set -e
set -u 

typeset -i i END

let END=$1

export SEA_HOME=/home/vhs/Sea
experiments=("${SEA_HOME}/sea_launch.sh sea_parallel.sh")
experiments+=("bash ./lustre_parallel.sh")
results_file=./results/result.csv
curr_exp=""

order=( 0 1 2 3 )

echo "experiment,repetition,runtime,max_mem" > ${results_file}

for ((i=0;i<END;i++))
do
    order=( $(shuf -e "${order[@]}") )
    for e in "${order[@]}"
    do
        echo $e $i
        rm /mnt/lustre/vhs/output/* sea.log ; rm -rf /disk0/vhs/seatmp /dev/shm/seatmp
        echo 3 | sudo tee /proc/sys/vm/drop_caches
        echo running experiment ${experiments[0]}

        exec 3>&1 4>&2

        if [[ $e == 0 ]]
        then
            curr_exp="mem_all"
            cp .sea_flushlist_all ${SEA_HOME}/.sea_flushlist
            echo "mem ${experiments[0]}" 
            cp mem_sea.ini ${SEA_HOME}/sea.ini
            res=$({ command time -f "%e,%M" ${experiments[0]} 1>&3; } 2>&1 | grep "[0-9]*\.[0-9]*,[0-9]*" )
        elif [[ $e == 2 ]]
        then
            curr_exp="mem_final"
            cp .sea_flushlist_mem ${SEA_HOME}/.sea_flushlist
            echo "mem_final ${experiments[0]}" 
            cp mem_sea.ini ${SEA_HOME}/sea.ini
            res=$({ command time -f "%e,%M" ${experiments[0]} 1>&3; } 2>&1 | grep "[0-9]*\.[0-9]*,[0-9]*" )
        else
            curr_exp="lustre"
            echo "lustre ${experiments[1]}"
            res=$({ command time -f "%e,%M" ${experiments[1]} 1>&-; } 2>&1 )
        fi
        exec 3>&- 4>&-
        echo "$curr_exp,$i,$res" >> ${results_file}
        sleep 20
        echo "done experiment $curr_exp repetition $i"
    done
done
