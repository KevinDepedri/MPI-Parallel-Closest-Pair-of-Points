#!/bin/bash
#PBS -l select=2:ncpus=8:mem=1gb -l place=scatter

# Set max execution time
#PBS -l walltime=2:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N 16Scatter
#PBS -o 16Scatter.txt
#PBS -e 16Scatter_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 16 /home/kevin.depedri/hpc3/runs/parallel/bruteforce/mpi_bruteforce /home/kevin.depedri/points/500K5d.txt -e -p