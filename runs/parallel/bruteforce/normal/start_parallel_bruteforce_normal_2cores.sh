#!/bin/bash
#PBS -l select=1:ncpus=2:mem=6gb

# Set max execution time
#PBS -l walltime=240:00:00

# Set the excution on the short queue
#PBS -q long_cpuQ

# Set name of job, output and error file
#PBS -N BruteForce2CoreParallel
#PBS -o BruteForce2CoreParallel.txt
#PBS -e BruteForce2CoreParallel_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 2 /home/kevin.depedri/hpc3/runs/parallel/bruteforce/mpi_bruteforce /home/kevin.depedri/points/100M5d.txt -e -p