#!/bin/bash
#PBS -l select=1:ncpus=4:mem=5gb

# Set max execution time
#PBS -l walltime=6:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N Rec4CoreNormal
#PBS -o Rec4CoreNormal.txt
#PBS -e Rec4CoreNormal_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 4 /home/kevin.depedri/hpc3/runs/parallel/recursive/mpi_recursive_short_250M5d