#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Structure for a point in any number of dimensions
typedef struct{
    int *coordinates;
    int num_dimensions;
} Point;

// Calculates the Euclidean distance between two points
double distance(Point p1, Point p2){
    double sum = 0;
    int n = p1.num_dimensions;
    for (int i = 0; i < n; i++){
        sum += pow((p1.coordinates[i] - p2.coordinates[i]), 2);
    }
    return sqrt(sum);
}

int main(int argc, char *argv[]){
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


    //print the first point
    printf("\nThe first point first coordinate is %d", points[0].coordinates[0]);
    printf("\nThe second point first coordinate is %d", points[1].coordinates[0]);
    // Calculate the distance between each pair of points
    
    Point p1;
    p1.num_dimensions = 2;
    p1.coordinates = (int *)malloc(2 * sizeof(int));
    p1.coordinates[0] = 0;
    p1.coordinates[1] = 1;

    Point p2;
    p2.num_dimensions = 2;
    p2.coordinates = (int *)malloc(2 * sizeof(int));
    p2.coordinates[0] = 1;
    p2.coordinates[1] = 2;

    double min_distance = distance(p1,p2);
    printf("The minimum distance is %f", min_distance);
    // free the memory
    free(points);
 
   return 0;
}