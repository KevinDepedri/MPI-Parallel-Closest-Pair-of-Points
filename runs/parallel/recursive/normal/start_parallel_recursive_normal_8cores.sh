#!/bin/bash
#PBS -l select=1:ncpus=8:mem=2gb

# Set max execution time
#PBS -l walltime=3:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N Rec8CoreNormal
#PBS -o Rec8CoreNormal.txt
#PBS -e Rec8CoreNormal_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 8 /home/kevin.depedri/hpc3/runs/parallel/recursive/mpi_recursive_short /home/kevin.depedri/points/250M5d.txt -e -p