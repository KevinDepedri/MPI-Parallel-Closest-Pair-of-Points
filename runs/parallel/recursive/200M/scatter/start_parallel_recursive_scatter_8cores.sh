#!/bin/bash
#PBS -l select=2:ncpus=4:mem=2gb -l place=scatter

# Set max execution time
#PBS -l walltime=3:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N 8Scatter
#PBS -o 8Scatter.txt
#PBS -e 8Scatter_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 8 /home/kevin.depedri/hpc3/runs/parallel/recursive/mpi_recursive_long /home/kevin.depedri/points/200M5d.txt -e -p