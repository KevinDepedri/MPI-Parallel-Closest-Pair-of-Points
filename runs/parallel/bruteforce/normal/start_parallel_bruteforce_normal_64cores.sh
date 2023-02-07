#!/bin/bash
#PBS -l select=1:ncpus=64:mem=408gb

# Set max execution time
#PBS -l walltime=24:00:00

# Set the excution on the short queue
#PBS -q long_cpuQ

# Set name of job, output and error file
#PBS -N BruteForce64CoreParallel
#PBS -o BruteForce64CoreParallel.txt
#PBS -e BruteForce64CoreParallel_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 64 /home/kevin.depedri/hpc3/runs/parallel/bruteforce/mpi_bruteforce /home/kevin.depedri/points/100M5d.txt -e -p