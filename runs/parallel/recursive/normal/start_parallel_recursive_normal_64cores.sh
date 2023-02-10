#!/bin/bash
#PBS -l select=1:ncpus=64:mem=2gb

# Set max execution time
#PBS -l walltime=1:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N 64CoreNormal
#PBS -o 64CoreNormal.txt
#PBS -e 64CoreNormal_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 64 /home/kevin.depedri/hpc3/runs/parallel/recursive/mpi_recursive_long /home/kevin.depedri/points/250M5d.txt -e -p