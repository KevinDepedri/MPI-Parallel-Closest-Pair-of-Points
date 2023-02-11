#!/bin/bash
#PBS -l select=1:ncpus=1:mem=1gb

# Set max execution time
#PBS -l walltime=10:00:00

# Set the excution on the short queue
#PBS -q long_cpuQ

# Set name of job, output and error file
#PBS -N 1CoreSeq
#PBS -o 1CoreSeq.txt
#PBS -e 1CoreSeq_error.txt

# Load the library and execute the parallel application
/home/kevin.depedri/hpc3/runs/sequential/bruteforce/sequential_bruteforce /home/kevin.depedri/points/500K5d.txt -e -p