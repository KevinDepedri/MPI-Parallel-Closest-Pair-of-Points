#!/bin/bash
#PBS -l select=1:ncpus=1:mem=5gb

# Set max execution time
#PBS -l walltime=6:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N Rec1CoreNormal
#PBS -o Rec1CoreNormal.txt
#PBS -e Rec1CoreNormal_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 1 /home/kevin.depedri/hpc3/runs/parallel/recursive/mpi_recursive_long_2cores /home/kevin.depedri/points/250M5d.txt