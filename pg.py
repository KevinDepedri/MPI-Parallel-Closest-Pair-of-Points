import random
import sys

# read from arguments the number of points and the number of dimensions
num_points = int(sys.argv[1])
num_dimensions = int(sys.argv[2])
name_file = str(sys.argv[3])
print(f'Generating {num_points} points in {num_dimensions} dimensions')

# Lower and upper bounds for the coordinates
lower_bound = -num_points
upper_bound = num_points

# create a list of lists with the coordinates
bowl = []
for i in range(num_dimensions):
    bowl.append(list(range(lower_bound, upper_bound)))
    bowl[i] = bowl[i] * (i+1)


# create random coordinates picking from the lists and removing the element
points = []

for i in range(num_points):
    point = []
    for j in range(num_dimensions):
        point.append(random.choice(bowl[j]))
        bowl[j].remove(point[j])
    points.append(point)


# Save to points.txt in the first line the number of points and the number of dimensions
# and in the following lines the coordinates of the points
with open(name_file+'.txt', 'w') as f:
    f.write(str(num_points) + ' ' + str(num_dimensions) +'\n')
    for i in range(num_points):
        f.write(' '.join(str(points[i][j]) for j in range(num_dimensions))+'\n')

print(f'Done')

