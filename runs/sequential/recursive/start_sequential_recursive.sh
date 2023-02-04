#!/bin/bash
#PBS -l select=1:ncpus=1:mem=20gb

# Set max execution time
#PBS -l walltime=120:00:00

# Set the excution on the short queue
#PBS -q long_cpuQ

# Set name of job, output and error file
#PBS -N Rec1CoreSeq
#PBS -o Rec1CoreSeq.txt
#PBS -e Rec1CoreSeq_error.txt

# Load the library and execute the parallel application
/home/kevin.depedri/hpc3/runs/sequential/recursive/sequential_recursive_250M5d