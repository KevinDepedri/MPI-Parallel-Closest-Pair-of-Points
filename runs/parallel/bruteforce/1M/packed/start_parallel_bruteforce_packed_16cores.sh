#!/bin/bash
#PBS -l select=2:ncpus=8:mem=2gb -l place=pack

# Set max execution time
#PBS -l walltime=3:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N 16Packed
#PBS -o 16Packed.txt
#PBS -e 16Packed_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 16 /home/kevin.depedri/hpc3/runs/parallel/bruteforce/mpi_bruteforce /home/kevin.depedri/points/1M5d.txt -e -p