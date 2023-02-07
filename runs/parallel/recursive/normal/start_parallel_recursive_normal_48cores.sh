#!/bin/bash
#PBS -l select=1:ncpus=48:mem=2gb

# Set max execution time
#PBS -l walltime=1:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N Rec48CoreNormal
#PBS -o Rec48CoreNormal.txt
#PBS -e Rec48CoreNormal_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 48 /home/kevin.depedri/hpc3/runs/parallel/recursive/mpi_recursive_short /home/kevin.depedri/points/250M5d.txt -e -p