#!/usr/bin/python
#PBS -l select=1:ncpus=1:mem=30gb
#PBS -N PGen500M5d
#PBS -o 500M5d.txt
#PBS -e 500M5d_error.txt
#PBS -q long_cpuQ 
#PBS -l walltime=18:00:00

import random

# Define number of points, number of dimensions and bound
num_points = 500000000
num_dimensions = 5
bound = 2500

# If the points cannot fit that dimension then return error
if((2*(bound)+1)**num_dimensions < num_points):
    print('Error: Too many points for the given bound and dimensions')
    exit()

# Compute lower and upper bounds for the coordinates
lower_bound = -bound
upper_bound = bound

# Define a set of points (no duplicate points)
points = set()

# Print the header of the file
print('{} {}'.format(num_points, num_dimensions))

# For each point that need to be generated
while(num_points > 0):
    point = []

    # Append the required number of coordinates
    for j in range(num_dimensions):
        point.append(random.randint(lower_bound, upper_bound))
    
    # If the point is new (not in set) the print it and add it to set
    if tuple(point) not in points:
        # Print multiple coordinate onthe same line and a new line after the last one
        for coordinate in point:
            print coordinate, ## Let it this way, it is python 2 way of printing on a single line
        print
        # Add the point to the set and decrement the counter
        points.add(tuple(point))
        num_points -= 1
