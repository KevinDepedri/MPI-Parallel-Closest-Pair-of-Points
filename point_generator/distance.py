point1 = (-2165, -2907, -1889, 4814, 2964)
point2 = (-2149, -2892, -1903, 4836, 2955)

def distance(point1, point2):
    return sum((x-y)**2 for x,y in zip(point1, point2))**0.5

print(distance(point1, point2))