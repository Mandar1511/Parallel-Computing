#!/bin/sh 
#SBATCH -N 16
#SBATCH --ntasks-per-node=48
#SBATCH -m plane=1
#SBATCH --time=00:10:20 
#SBATCH --job-name=dissbar
#SBATCH --error=_job.%J.err
#SBATCH --output=_job.%J.out
#SBATCH --partition=standard 
#module load compiler/intel/2018.2.199 
module load compiler/intel/2020.4.304
export I_MPI_FABRICS=shm:ofi 
cd $SLURM_SUBMIT_DIR


# Compile
mpiicpc -std=c++11 -O3 -g red.cpp -o red
# Check if SLURM_NTASKS is set
if [ -z "$SLURM_NTASKS" ]; then
    echo "SLURM_NTASKS is not set. Please set it or run this script in a SLURM environment."
    exit 1
fi

echo "Now running SCATTER assignment"
# This is SCATTER assignment.
# Start with 1 task and double until reaching SLURM_NTASKS
num_tasks=1
while [ $num_tasks -le $SLURM_NTASKS ]; do
    echo "Launching command with $num_tasks tasks..."
    # Calculate -ppn
    if [ $num_tasks -le $SLURM_NNODES ]; then
        ppn=1
    else
        ppn=$(($num_tasks / $SLURM_NNODES))
        if [ $(($num_tasks % $SLURM_NNODES)) -ne 0 ]; then
            ppn=$(($ppn + 1))
        fi
    fi
    time mpiexec.hydra -n $num_tasks -ppn $ppn ./red
    # Double the number of tasks for the next iteration
    num_tasks=$((num_tasks * 2))
done

echo "Now running COMPACT assignment"
# This is COMPACT assignment
# Start with 1 task and double until reaching SLURM_NTASKS
num_tasks=1
while [ $num_tasks -le $SLURM_NTASKS ]; do
    echo "Launching command with $num_tasks tasks..."
    ppn=$SLURM_NTASKS_PER_NODE
    #ppn=48
    time mpiexec.hydra -n $num_tasks -ppn $ppn ./red
    # Double the number of tasks for the next iteration
    num_tasks=$((num_tasks * 2))
done


echo "Done."
