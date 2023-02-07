#!/bin/bash
#PBS -l select=1:ncpus=4:mem=28gb

# Set max execution time
#PBS -l walltime=24:00:00

# Set the excution on the short queue
#PBS -q long_cpuQ

# Set name of job, output and error file
#PBS -N BruteForce4CoreParallel
#PBS -o BruteForce4CoreParallel.txt
#PBS -e BruteForce4CoreParallel_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 4 /home/kevin.depedri/hpc3/runs/parallel/bruteforce/mpi_bruteforce