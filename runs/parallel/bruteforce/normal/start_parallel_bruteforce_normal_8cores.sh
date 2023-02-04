#!/bin/bash
#PBS -l select=1:ncpus=8:mem=10gb

# Set max execution time
#PBS -l walltime=24:00:00

# Set the excution on the short queue
#PBS -q long_cpuQ

# Set name of job, output and error file
#PBS -N BruteForce8CoreParallel
#PBS -o BruteForce8CoreParallel.txt
#PBS -e BruteForce8CoreParallel_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 8 /home/kevin.depedri/hpc3/runs/parallel/bruteforce/mpi_bruteforce