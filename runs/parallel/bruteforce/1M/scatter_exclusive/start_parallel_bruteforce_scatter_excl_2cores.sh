#!/bin/bash
#PBS -l select=2:ncpus=1:mem=1gb -l place=scatter:excl

# Set max execution time
#PBS -l walltime=15:00:00

# Set the excution on the short queue
#PBS -q long_cpuQ

# Set name of job, output and error file
#PBS -N 2ScatterExclusive
#PBS -o 2ScatterExclusive.txt
#PBS -e 2ScatterExclusive_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 2 /home/kevin.depedri/hpc3/runs/parallel/bruteforce/mpi_bruteforce /home/kevin.depedri/points/1M5d.txt -e -p