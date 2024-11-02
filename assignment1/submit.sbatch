#!/bin/bash
#SBATCH --job-name=CILK_JOB    	      # Job name
#SBATCH --mail-type=END,FAIL          # Mail events (NONE, BEGIN, END, FAIL, ALL)
#SBATCH --mail-user=email@iitdh.ac.in # Where to send mail	
#SBATCH --nodes=1                     # Run on a single node
#SBATCH --ntasks-per-node=1           # Single task
#SBATCH --cpus-per-task=32            # 32 CPUs per task
#SBATCH --time=00:10:00               # Time limit hrs:min:sec
#SBATCH --output=serial_test_%j.log   # Standard output and error log
#SBATCH --partition=v100   # Queuename
pwd; hostname; date

set -ex
make clean
make all
make runSortCilk CILK_NWORKERS=32 SIZE=2000000000 CUTOFF=0
make runSortCilk CILK_NWORKERS=32 SIZE=2000000000 CUTOFF=1
make runSortCilk CILK_NWORKERS=32 SIZE=2000000000 CUTOFF=2
make runSortCilk CILK_NWORKERS=32 SIZE=2000000000 CUTOFF=3
make runSortCilk CILK_NWORKERS=32 SIZE=2000000000 CUTOFF=4
make runSortCilk CILK_NWORKERS=32 SIZE=2000000000 CUTOFF=5
make runSortCilk CILK_NWORKERS=32 SIZE=2000000000 CUTOFF=6
make runSortCilk CILK_NWORKERS=32 SIZE=2000000000 CUTOFF=7
make runSortCilk CILK_NWORKERS=32 SIZE=2000000000 CUTOFF=8
make runSortCilk CILK_NWORKERS=32 SIZE=2000000000 CUTOFF=16
make runSortCilk CILK_NWORKERS=32 SIZE=2000000000 CUTOFF=32
make
date