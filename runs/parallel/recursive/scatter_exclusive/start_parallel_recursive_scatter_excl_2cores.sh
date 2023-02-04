#!/bin/bash
#PBS -l select=2:ncpus=1:mem=2gb -l place=scatter:excl

# Set max execution time
#PBS -l walltime=1:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N Rec2CoreScatterExcl
#PBS -o Rec2CoreScatterExcl.txt
#PBS -e Rec2CoreScatterExcl_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 2 /home/kevin.depedri/runs/parallel/recursive/mpi_recursive