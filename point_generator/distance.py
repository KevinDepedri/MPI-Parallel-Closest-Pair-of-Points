point1 = (1,2,3,4,5)
point2 = (2,3,4,5,6)

def distance(point1, point2):
    return sum((x-y)**2 for x,y in zip(point1, point2))**0.5

print(distance(point1, point2))