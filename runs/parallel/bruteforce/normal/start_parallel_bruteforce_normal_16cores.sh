#!/bin/bash
#PBS -l select=1:ncpus=16:mem=102gb

# Set max execution time
#PBS -l walltime=6:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N BruteForce16CoreParallel
#PBS -o BruteForce16CoreParallel.txt
#PBS -e BruteForce16CoreParallel_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 16 /home/kevin.depedri/hpc3/runs/parallel/bruteforce/mpi_bruteforce