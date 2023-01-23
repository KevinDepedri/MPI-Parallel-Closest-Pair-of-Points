#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Structure for a point in any number of dimensions
typedef struct
{
    int *coordinates;
    int num_dimensions;
} Point;

// Calculates the Euclidean distance between two points
double distance(Point p1, Point p2)
{
    double sum = 0;
    for (int i = 0; i < p1.num_dimensions; i++)
    {
        sum += (p1.coordinates[i] - p2.coordinates[i]) * (p1.coordinates[i] - p2.coordinates[i]);
    }
    return sqrt(sum);
}
int main(int argc, char *argv[])
{
    Point* points;
    int num_points, num_dimensions;
    FILE* fp = fopen("points.txt", "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    // Read the number of points and dimensions from the first line of the file
    fscanf(fp, "%d %d", &num_points, &num_dimensions);
    // Allocate memory for the points
    points = (Point *)malloc(num_points * sizeof(Point));
    for (int i = 0; i < num_points; i++)
    {
        points[i].coordinates = (int *)malloc(num_dimensions * sizeof(double));
        for (int j = 0; j < num_dimensions; j++)
        {
            fscanf(fp, "%d", &points[i].coordinates[j]);
        }
    }
    fclose(fp);
    printf("The number of points is %d", num_points);
    
    
    // Calculate the distance between each pair of points
    double min_dist = distance(points[0], points[1]);
    for (int i = 0; i < num_points; i++)
    {
        for (int j = i + 1; j < num_points; j++)
        {
            double dist = distance(points[i], points[j]);
            if (dist < min_dist)
            {
                min_dist = dist;
            }
        }
    }
    printf("The closest pair of points' distance is %f", min_dist);
    // free the memory
    free(points);
 
   return 0;
}