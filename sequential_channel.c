#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Structure for a point in any number of dimensions
typedef struct
{
    double *coordinates;
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
    int num_points;
    FILE* fp = fopen("points.txt", "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    // Read the number of points and dimensions from the first line of the file
    fscanf(fp, "%d %d", &num_points, &points[0].num_dimensions);
    // Allocate memory for the points
    points = (Point *)malloc(num_points * sizeof(Point));
    for (int i = 0; i < num_points; i++)
    {
        points[i].coordinates = (double *)malloc(points[0].num_dimensions * sizeof(double));
    }
    // Read the coordinates of the points from the file
    for (int i = 0; i < num_points; i++)
    {
        for (int j = 0; j < points[0].num_dimensions; j++)
        {
            fscanf(fp, "%lf", &points[i].coordinates[j]);
        }
    }
    // Close the file
    fclose(fp);
    return 0;
}