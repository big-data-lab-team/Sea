#!/bin/bash

experiments=()
experiments+=("bash sea_launch.sh python increment.py /mnt/lustre/vhs/tmp_bb /mnt/lustre/vhs/mount 20")
experiments+=("bash sea_launch.sh python increment.py /mnt/lustre/vhs/tmp_bb /mnt/lustre/vhs/output 20")
experiments+=("bash sea_launch.sh python increment.py /mnt/lustre/vhs/tmp_bb /dev/shm/seatmp 20")

export SEA_HOME=$PWD
for e in {0..2}
do
    for i in {1..3}
    do
        echo $e $i
        rm /mnt/lustre/vhs/output/* sea.log ; rm -rf /disk0/vhs/seatmp /dev/shm/seatmp
        echo 3 | sudo tee /proc/sys/vm/drop_caches
        echo running experiment ${experiments[$e]}
        if [[ $e == 0 ]]
        then
            export LD_PRELOAD=$PWD/passthrough.so
            echo LD_PRELOAD ${LD_PRELOAD}
            time ${experiments[$e]}
            unset LD_PRELOAD
        else
            time ${experiments[$e]}
        fi 
        sleep 20
        echo 'continuing'
        break
    done
    break
done
