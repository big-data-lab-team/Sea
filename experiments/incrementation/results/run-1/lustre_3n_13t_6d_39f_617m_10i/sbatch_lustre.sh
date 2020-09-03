#!/bin/bash
#SBATCH --time=90:55:00
#SBATCH --account=vhs
#SBATCH --job-name=lustre_3n_13t_6d_39f_617m_10i
#SBATCH --nodes=3
#SBATCH --output=./results/run-1/lustre_3n_13t_6d_39f_617m_10i/slurm-%x-%j.out

source /home/vhs/Sea/.venv/bin/activate
    

srun -N3 echo "Clearing cache" && sync && echo 3 | sudo tee /proc/sys/vm/drop_caches
    
start=`date +%s.%N`
srun -N 1 bash ./results/run-1/lustre_3n_13t_6d_39f_617m_10i/n0_sea_parallel.sh &
srun -N 1 bash ./results/run-1/lustre_3n_13t_6d_39f_617m_10i/n1_sea_parallel.sh &
srun -N 1 bash ./results/run-1/lustre_3n_13t_6d_39f_617m_10i/n2_sea_parallel.sh &
wait

end=`date +%s.%N`

runtime=$( echo "$end - $start" | bc -l )

echo "Runtime: $runtime"
