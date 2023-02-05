#!/bin/bash
#PBS -l select=1:ncpus=85:mem=2gb

# Set max execution time
#PBS -l walltime=1:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N Rec85CoreNormal
#PBS -o Rec85CoreNormal.txt
#PBS -e Rec85CoreNormal_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 85 /home/kevin.depedri/hpc3/runs/parallel/recursive/mpi_recursive_short_250M5d