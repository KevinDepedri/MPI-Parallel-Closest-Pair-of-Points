#!/bin/bash
#PBS -l select=2:ncpus=12:mem=2gb -l place=pack:excl

# Set max execution time
#PBS -l walltime=1:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N 24PackedExcl
#PBS -o 24PackedExcl.txt
#PBS -e 24PackedExcl_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 24 /home/kevin.depedri/hpc3/runs/parallel/recursive/mpi_recursive_long /home/kevin.depedri/points/50M5d.txt -e -p