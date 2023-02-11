#!/bin/bash
#PBS -l select=2:ncpus=32:mem=2gb -l place=pack:excl

# Set max execution time
#PBS -l walltime=1:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N 64PackedExclusive
#PBS -o 64PackedExclusive.txt
#PBS -e 64PackedExclusive_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 64 /home/kevin.depedri/hpc3/runs/parallel/bruteforce/mpi_bruteforce /home/kevin.depedri/points/500K5d.txt -e -p