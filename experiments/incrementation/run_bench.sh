#!/bin/bash

#set -e
set -u 

export SEA_HOME=/home/vhs/Sea
experiments=("${SEA_HOME}/sea_launch.sh sea_parallel.sh")
experiments+=("bash ./lustre_parallel.sh")
for e in {0..2}
do
    for i in {1..3}
    do
        echo $e $i
        rm /mnt/lustre/vhs/output/* sea.log ; rm -rf /disk0/vhs/seatmp /dev/shm/seatmp
        echo 3 | sudo tee /proc/sys/vm/drop_caches
        echo running experiment ${experiments[0]}
        if [[ $e == 0 ]]
        then
            echo "mem ${experiments[0]}" 
            cp mem_sea.ini ${SEA_HOME}/sea.ini
            time ${experiments[0]}
        elif [[ $e == 1 ]]
        then
            echo "disk ${experiments[0]}"
            cp disk_sea.ini ${SEA_HOME}/sea.ini
            time ${experiments[0]}
        else
            echo "lustre ${experiments[1]}"
            time ${experiments[1]}
        fi 
        echo 'done'
        sleep 20
        echo 'continuing'
    done
done
