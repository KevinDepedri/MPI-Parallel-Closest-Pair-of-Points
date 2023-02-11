#!/bin/bash
#PBS -l select=2:ncpus=2:mem=1gb -l place=scatter:excl

# Set max execution time
#PBS -l walltime=5:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N 4ScatterExclusive
#PBS -o 4ScatterExclusive.txt
#PBS -e 4ScatterExclusive_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 4 /home/kevin.depedri/hpc3/runs/parallel/bruteforce/mpi_bruteforce /home/kevin.depedri/points/1M5d.txt -e -p