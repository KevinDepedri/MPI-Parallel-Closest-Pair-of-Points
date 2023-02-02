# generate 20 different points in a 2D plane that in the [-10, 10] range
# sort them over x
# plot them in a scatterplot
# draw a vertical line at each half of the range
# each line is green and darker as it gets closer to the center

import random
import sys
import matplotlib.pyplot as plt

n_points = 20
n_dimensions = 2
bound = 10

points = set()
while(len(points) < n_points):
    point = []
    for j in range(n_dimensions):
        point.append(random.randint(-bound, bound))
    if tuple(point) not in points:
        points.add(tuple(point))
# sort the points over x
points = sorted(points, key=lambda p: p[0])

x = [p[0] for p in points]
y = [p[1] for p in points]
plt.scatter(x, y)

# force the plot to be from -bound to bound in both x and y
plt.xlim(-bound, bound)
plt.ylim(-bound, bound)
# force the plot to be square
plt.gca().set_aspect('equal', adjustable='box')

# draw a vertical line on half and half and half of the range
def draw_line(x, color, alpha):
    plt.axvline(x, color=color, alpha=alpha)
    
draw_line(0, 'green', 0.8)

# now draw a thicker grey line but with a different alpha 4px wide
plt.axvline(0, color='grey', alpha=0.2, linewidth=25)



plt.show()


plt.show()