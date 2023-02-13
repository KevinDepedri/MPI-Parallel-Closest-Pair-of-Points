#!/bin/bash
#PBS -l select=2:ncpus=1:mem=10gb -l place=scatter

# Set max execution time
#PBS -l walltime=6:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N 2Scatter
#PBS -o 2Scatter.txt
#PBS -e 2Scatter_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 2 /home/kevin.depedri/hpc3/runs/parallel/recursive/mpi_recursive_long_2cores /home/kevin.depedri/points/50M5d.txt