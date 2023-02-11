#!/bin/bash
#PBS -l select=2:ncpus=12:mem=2gb -l place=pack:excl

# Set max execution time
#PBS -l walltime=2:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N 24PackedExclusive
#PBS -o 24PackedExclusive.txt
#PBS -e 24PackedExclusive_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 24 /home/kevin.depedri/hpc3/runs/parallel/bruteforce/mpi_bruteforce /home/kevin.depedri/points/1M5d.txt -e -p