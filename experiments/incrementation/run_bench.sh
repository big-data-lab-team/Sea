#!/bin/bash

#set -e
set -u 

typeset -i i END

let END=$1
results_file=$2

export SEA_HOME=/home/vhs/Sea
experiments=("${SEA_HOME}/bin/sea_launch.sh sea_parallel.sh")
experiments+=("bash ./lustre_parallel.sh")
all_out=./results/mem_all.out
flush_out=./results/flushlist.out
base_dir=/mnt/lustre/vhs/output
time_params="%e,%M"
curr_exp=""

order=( 0 1 2 )

echo "experiment,repetition,runtime,max_mem,flush_time,disk_files,total_flush" > ${results_file}

for ((i=0;i<END;i++))
do
    order=( $(shuf -e "${order[@]}") )
    for e in "${order[@]}"
    do
        echo $e $i
        echo "Clearing cache"
        sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
        echo "Removing directories"
        rm /mnt/lustre/vhs/output/*;
    	rm -rf /dev/shm/seatmp
	    rm -rf /disk0/vhs/seatmp /disk1/vhs/seatmp /disk2/vhs/seatmp /disk3/vhs/seatmp /disk4/vhs/seatmp /disk5/vhs/seatmp /dev/shm/seatmp 
        echo "Creating temp source mount directories"
        mkdir /dev/shm/seatmp
	    mkdir /disk0/vhs/seatmp /disk1/vhs/seatmp /disk2/vhs/seatmp /disk3/vhs/seatmp /disk4/vhs/seatmp /disk5/vhs/seatmp
        echo running experiment ${experiments[0]}

        exec 3>&1 4>&2

        if [[ $e == 0 ]]
        then
            curr_exp="mem_all"
            cp .sea_flushlist_all ${SEA_HOME}/.sea_flushlist
            echo "mem ${experiments[0]}" 
            cp mem_sea.ini ${SEA_HOME}/sea.ini
            output=$( command time -f ${time_params} ${experiments[0]} &> ${all_out} )
            cat ${all_out} | grep -i "flushing*" > ${flush_out}
            res=$(cat ${all_out} | grep "[0-9]*\.[0-9]*,[0-9]*" )

            while [[ $(ls ${base_dir} | wc -l) != 390 ]]; do sleep 10; done

            flushtime=$(cat results/mem_all.out | grep 'real' | grep "[0-9].*")
            flushtime=$(echo ${flushtime:5})
            ssdwrites=$(cat ${flush_out} | grep 'disk'| wc -l)
            totalflush=$(cat ${flush_out} | wc -l)
        elif [[ $e == 1 ]]
        then
            curr_exp="mem_final"
            cp .sea_flushlist_mem ${SEA_HOME}/.sea_flushlist
            echo "mem_final ${experiments[0]}" 
            cp mem_sea.ini ${SEA_HOME}/sea.ini
            output=$( command time -f ${time_params} ${experiments[0]} &> ${all_out} )
            cat ${all_out} | grep -i "flushing*" > ${flush_out}
            res=$(cat ${all_out} | grep "[0-9]*\.[0-9]*,[0-9]*" )

            while [[ $(ls ${base_dir} | wc -l) != 39 ]]; do sleep 10; done

            flushtime=$(cat results/mem_all.out | grep 'real' | grep "[0-9].*")
            flushtime=$(echo ${flushtime:5})
            ssdwrites=$(cat ${flush_out} | grep 'disk'| wc -l)
            totalflush=$(cat ${flush_out} | wc -l)
        else
            curr_exp="lustre"
            echo "lustre ${experiments[1]}"
            res=$({ command time -f ${time_params} ${experiments[1]} 1>&-; } 2>&1 )
            flushtime=0
            ssdwrites=0
        fi
        exec 3>&- 4>&-
        echo "$curr_exp,$i,$res,$flushtime,$ssdwrites,$totalflush" >> ${results_file}
        sleep 20
        echo "done experiment $curr_exp repetition $i"
    done
done
