#!/bin/bash
#PBS -l select=2:ncpus=12:mem=1gb -l place=pack

# Set max execution time
#PBS -l walltime=2:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N 24Packed
#PBS -o 24Packed.txt
#PBS -e 24Packed_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 24 /home/kevin.depedri/hpc3/runs/parallel/bruteforce/mpi_bruteforce /home/kevin.depedri/points/500K5d.txt -e -p