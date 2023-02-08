#!/bin/bash
#PBS -l select=2:ncpus=12:mem=2gb -l place=pack

# Set max execution time
#PBS -l walltime=1:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N Rec24CorePacked
#PBS -o Rec24CorePacked.txt
#PBS -e Rec24CorePacked_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 24 /home/kevin.depedri/hpc3/runs/parallel/recursive/mpi_recursive_short /home/kevin.depedri/points/250M5d.txt -e -p