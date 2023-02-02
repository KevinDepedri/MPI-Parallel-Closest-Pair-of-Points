point1 = (-997, 88, 971, 52, -857)
point2 = (-988, 100, 970, 61, -885)

def distance(point1, point2):
    return sum((x-y)**2 for x,y in zip(point1, point2))**0.5

print(distance(point1, point2))