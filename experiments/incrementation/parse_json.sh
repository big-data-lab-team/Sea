#!/bin/bash

cond_file=$1

out_fldr="/mnt/lustre/vhs/output"
sea_mount="/mnt/lustre/vhs/mount"
rdir="./results"
library="/home/vhs/Sea/build/sea.so"
executable="python increment.py"

mkdir -p ${rdir}
ncond=$(jq ". | length" ${cond_file})


main () {
    echo "Preparing experiment launch"

    for ((i=0;i<ncond;i++))
    do
        name=$(jq -r ".[$i] .name" ${cond_file})
        ndisks=$(jq ".[$i] .ndisks" ${cond_file})
        nnodes=$(jq ".[$i] .nnodes" ${cond_file})
        nthreads=$(jq ".[$i] .nthreads" ${cond_file})
        nfiles=$(jq ".[$i] .nfiles" ${cond_file})
        in_fldr=$(jq -r ".[$i] .in_fldr" ${cond_file})
        niterations=$(jq ".[$i] .niterations" ${cond_file})
        strategy=$(jq -r ".[$i] .strategy" ${cond_file})

        echo "Experiment 1 Run 0 conditions
name: $name
strategy: $strategy
file input folder: ${in_fldr}
number of disks: $ndisks
number of nodes: $nnodes
number of threads: $nthreads
number of files: $nfiles
number of iterations: ${niterations}"
        exp_fldr="${rdir}/${name}"
        rm -rf $exp_fldr/*
        echo -e "Experiment directory: ${exp_fldr}\n"

        fnode=$(($nfiles / $nnodes))
        rem=$(($nfiles % $nnodes))

        if [[ $strategy == "lustre" ]]
        then
            prefix=$executable
            out_mount=$out_fldr

        else
            prefix="LD_PRELOAD=$library $executable"
            out_mount=$sea_mount
        fi

        fcount=0
        node=0
        nprev=-1
        echo "Files to be executed per node: $fnode  remainder: $rem"

        job_dir="${exp_fldr}/jobs"
        mkdir -p ${job_dir}

        sl_script="${exp_fldr}/sbatch_${strategy}.sh"

        echo '#!/bin/bash
#SBATCH --time=90:55:00
#SBATCH --account=vhs
#SBATCH --job-name='"$name"'
#SBATCH --nodes='"$nnodes"'
#SBATCH --output='"${exp_fldr}"'slurm-%x-%j.out

source /home/vhs/Sea/.venv/bin/activate
    ' > ${sl_script}

        if [[ $strategy == "lustre" ]]
        then
            echo '
srun -N'"$nnodes"' echo "Clearing cache" && sync && echo 3 | sudo tee /proc/sys/vm/drop_caches
    ' >> ${sl_script}
        else
            echo 'SEA_HOME=/home/vhs/Sea

srun -N'"$nnodes"' rm -rf /disk0/vhs/seatmp /disk1/vhs/seatmp /disk2/vhs/seatmp /disk3/vhs/seatmp /disk4/vhs/seatmp /disk5/vhs/seatmp /dev/shm/seatmp
srun -N'"$nnodes"' echo "Clearing cache" && sync && echo 3 | sudo tee /proc/sys/vm/drop_caches

    echo "Creating temp source mount directories"
srun -N'"$nnodes"' mkdir /dev/shm/seatmp
srun -N'"$nnodes"' mkdir /disk0/vhs/seatmp /disk1/vhs/seatmp /disk2/vhs/seatmp /disk3/vhs/seatmp /disk4/vhs/seatmp /disk5/vhs/seatmp' >> ${sl_script}
        fi

        echo 'start=`date +%s.%`' >> ${sl_script}

        # flag to see if remainder was allotted to node (if applicable)
        if [[ $rem == 0 ]]
        then
            node_rem=true
        else
            node_rem=false
        fi

        for j in $(ls ${in_fldr} | head -n ${nfiles})
        do

            if [[ $fcount -ge $fnode ]]
            then

                if [[ $(($node + 1)) != $nnodes && ${node_rem} == true ]]
                then
                    echo "Number of files processed by node $node: $fcount"
                    fcount=0
                    node=$(($node + 1))

                    if [[ rem -gt 0 ]]
                    then
                        node_rem=false
                    fi
                else
                    rem=$(($rem - 1))
                    node_rem=true
                fi
            fi

            node_jobs="${job_dir}/jobs_n${node}.txt"
            echo "$prefix ${in_fldr}$j ${out_mount}" >> ${node_jobs}

            parallel_script=${exp_fldr}/n${node}_sea_parallel.sh

            if [[ $node != $nprev ]]
            then 
                echo "parallel --jobs ${nthreads} < ${node_jobs}" >> ${parallel_script}

                if [[ ${strategy} == "lustre" ]]
                then
                    echo "srun -N 1 ${parallel_script} &" >> ${sl_script}
                else
                    echo "srun -N 1 ${SEA_HOME}/bin/sea_launch.sh ${parallel_script} &" >> ${sl_script}
                fi
            fi
            nprev=$node

            fcount=$(($fcount + 1))
        done
        # for final node
        echo -e "Number of files processed by node $node: $fcount\n"

        echo 'end=`date +%s.%N`

runtime=$( echo "$end - $start" | bc -l )

echo "Runtime: $runtime"' >> ${sl_script}

        if [[ $strategy != "lustre" ]]
        then
            echo 'echo "Removing directories"
srun -N'"${nnodes}"' rm -rf /disk0/vhs/seatmp /disk1/vhs/seatmp /disk2/vhs/seatmp /disk3/vhs/seatmp /disk4/vhs/seatmp /disk5/vhs/seatmp /dev/shm/seatmp' >> ${sl_script}
        fi

        echo "Removing files from output directory"
        rm ${out_fldr}/*;

    done
}
main
