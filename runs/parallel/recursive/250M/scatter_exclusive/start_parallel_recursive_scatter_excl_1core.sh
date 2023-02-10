#!/bin/bash
#PBS -l select=1:ncpus=1:mem=5gb -l place=scatter:excl

# Set max execution time
#PBS -l walltime=6:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N 1ScatterExcl
#PBS -o 1ScatterExcl.txt
#PBS -e 1ScatterExcl_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 1 /home/kevin.depedri/hpc3/runs/parallel/recursive/mpi_recursive_long_2cores /home/kevin.depedri/points/250M5d.txt