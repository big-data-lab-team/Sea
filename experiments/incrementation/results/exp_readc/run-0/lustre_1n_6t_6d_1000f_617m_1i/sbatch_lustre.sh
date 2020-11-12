#!/bin/bash
#SBATCH --time=90:55:00
#SBATCH --account=vhs
#SBATCH --job-name=lustre_1n_6t_6d_1000f_617m_1i
#SBATCH --nodes=1
#SBATCH --output=./results/exp_readc/run-0/lustre_1n_6t_6d_1000f_617m_1i/slurm-%x-%j.out

source /home/vhs/Sea/.venv/bin/activate
    

srun -N1 ../scripts/clear_client_pc.sh
    
start=`date +%s.%N`
srun -N 1 bash ./results/exp_readc/run-0/lustre_1n_6t_6d_1000f_617m_1i/n0_sea_parallel.sh &
wait

end=`date +%s.%N`

runtime=$( echo "$end - $start" | bc -l )

echo "Runtime: $runtime"