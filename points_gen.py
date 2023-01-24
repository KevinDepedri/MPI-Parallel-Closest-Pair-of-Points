import random
import sys

# read from arguments the number of points and the number of dimensions
num_points = int(sys.argv[1])
num_dimensions = int(sys.argv[2])
bound = int(sys.argv[3])
name_file = str(sys.argv[4])
print(f'Generating {num_points} points in {num_dimensions} dimensions')

# Lower and upper bounds for the coordinates
lower_bound = -bound
upper_bound = bound

points = set()
while(num_points > 0):
    point = []
    # create integer coordinates
    for j in range(num_dimensions):
        point.append(random.randint(lower_bound, upper_bound))
    # add the point to the set if it is not already there
    if tuple(point) not in points:
        points.add(tuple(point))
        num_points -= 1

# Save to points.txt in the first line the number of points and the number of dimensions
# and in the following lines the coordinates of the points
with open(name_file+'.txt', 'w') as f:
    f.write(str(len(points)) + ' ' + str(num_dimensions) +'\n')
    for point in points:
        f.write(' '.join(str(x) for x in point)+'\n')

import matplotlib.pyplot as plt
if num_dimensions == 2:
    x = [p[0] for p in points]
    y = [p[1] for p in points]
    plt.scatter(x, y)
    plt.show()



print(f'Done')

