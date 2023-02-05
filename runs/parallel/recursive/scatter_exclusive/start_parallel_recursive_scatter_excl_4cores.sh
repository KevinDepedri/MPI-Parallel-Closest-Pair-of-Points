#!/bin/bash
#PBS -l select=2:ncpus=2:mem=5gb -l place=scatter:excl

# Set max execution time
#PBS -l walltime=6:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N Rec4CoreScatterExcl
#PBS -o Rec4CoreScatterExcl.txt
#PBS -e Rec4CoreScatterExcl_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 4 /home/kevin.depedri/hpc3/runs/parallel/recursive/mpi_recursive_short_250M5d