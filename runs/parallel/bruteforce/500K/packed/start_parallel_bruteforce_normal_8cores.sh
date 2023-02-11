#!/bin/bash
#PBS -l select=2:ncpus=4:mem=1gb -l place=pack

# Set max execution time
#PBS -l walltime=2:00:00

# Set the excution on the short queue
#PBS -q short_cpuQ

# Set name of job, output and error file
#PBS -N 8Packed
#PBS -o 8Packed.txt
#PBS -e 8Packed_error.txt

# Load the library and execute the parallel application
module load mpich-3.2
mpiexec -n 8 /home/kevin.depedri/hpc3/runs/parallel/bruteforce/mpi_bruteforce /home/kevin.depedri/points/500K5d.txt -e -p