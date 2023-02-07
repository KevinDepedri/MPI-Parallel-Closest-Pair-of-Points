#!/bin/bash
#PBS -l select=1:ncpus=1:mem=100gb

# Set max execution time
#PBS -l walltime=240:00:00

# Set the excution on the short queue
#PBS -q long_cpuQ

# Set name of job, output and error file
#PBS -N BruteForce1CoreSeq
#PBS -o BruteForce1CoreSeq.txt
#PBS -e BruteForce1CoreSeq_error.txt

# Load the library and execute the parallel application
/home/kevin.depedri/hpc3/runs/sequential/bruteforce/start_sequential_bruteforce.sh /home/kevin.depedri/points/100M5d.txt -e -p