import random
import sys

# read from arguments the number of points and the number of dimensions
num_points = int(sys.argv[1])
num_dimensions = int(sys.argv[2])
bound = int(sys.argv[3])
name_file = str(sys.argv[4])
plot = str(sys.argv[4])
if((2*(bound)+1)**num_dimensions < num_points):
    print(f'Error: Too many points for the given bound and dimensions')
    exit()
print(f'Generating {num_points} points in {num_dimensions} dimensions')
num_points2 = num_points

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

if plot:
    import matplotlib.pyplot as plt
    if num_dimensions == 2:
        x = [p[0] for p in points]
        y = [p[1] for p in points]
        plt.title(f'Distribution of {num_points2}points in {num_dimensions}D with a bound of {bound}')
        plt.scatter(x, y)
        plt.axvline(x = 0, color = 'red')
        plt.savefig(f'{name_file}.png')
        plt.show()

print(f'Done')
