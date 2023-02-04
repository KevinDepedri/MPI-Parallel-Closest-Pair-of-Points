#!/bin/bash
#PBS -l select=1:ncpus=80:mem=2gb

# Set max execution time
#PBS -l walltime=1:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N Rec80CoreNormal
#PBS -o Rec80Core.txt
#PBS -e Rec80Core_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 80 /home/kevin.depedri/hpc3/runs/parallel/recursive/mpi_recursive_short_250M5d