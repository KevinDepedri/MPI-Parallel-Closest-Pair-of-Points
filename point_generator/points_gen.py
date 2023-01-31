import random
import sys

# read from arguments the number of points and the number of dimensions
num_points = int(sys.argv[1])
num_dimensions = int(sys.argv[2])
bound = int(sys.argv[3])
name_file = str(sys.argv[4])

if((2*(bound)+1)**num_dimensions < num_points):
    print(f'Error: Too many points for the given bound and dimensions')
    exit()
print(f'Generating {num_points} points in {num_dimensions} dimensions')
num_points2 = num_points

# Lower and upper bounds for the coordinates
lower_bound = -bound
upper_bound = bound
perc1 = num_points/100
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
    # print progress
    if num_points % perc1 == 0:
        print(f'{num_points} points remaining')

# Save to points.txt in the first line the number of points and the number of dimensions
# and in the following lines the coordinates of the points, adding 0s to match the largest number of digits
with open(f'{name_file}.txt', 'w') as f:
    f.write(f'{num_points2} {num_dimensions}\n')

    max_digits = len(str(max([max(p) for p in points])))
    # add 0s to the front to match the largest number of digits
    for p in points:
        for c in p:
            f.write(f'{str(c).zfill(max_digits)} ')
        f.write('\n')

    f.close()

#plot the points if in 2D
import matplotlib.pyplot as plt
if num_dimensions == 2:
    x = [p[0] for p in points]
    y = [p[1] for p in points]
    plt.scatter(x, y)
    # save the plot as png
    plt.savefig(f'{name_file}.png')

print(f'Done')