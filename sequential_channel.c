#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Structure for a point in any number of dimensions
typedef struct Point{
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

// Run the main
int main(int argc, char *argv[])
{   
    printf("START OF PROGRAM\n");
    Point* points;
    int num_points, num_dimensions;

    FILE* fp = fopen("points.txt", "r");
    if (fp == NULL)
    {
        perror("Error opening file\n");
        return 1;
    } else { 
        printf("File opened correctly\n");
    }

    // Read the number of points and dimensions from the first line of the file
    fscanf(fp, "%d %d", &num_points, &num_dimensions);

    // Allocate memory for the points
    points = (Point *)malloc(num_points * sizeof(Point));
    for (int i = 0; i < num_points; i++)
    {   
        points[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
        for (int j = 0; j < num_dimensions; j++)
        {
            fscanf(fp, "%d", &points[i].coordinates[j]);
        }
    }
    fclose(fp);
    printf("File closed correctly\n");

    // Print the points
    for (int i = 0; i < num_points; i++)
    {
        printf("Point %d: (", i);
        for (int j = 0; j < num_dimensions; j++)
        {   
            if (j != num_dimensions-1){
                printf("%d, ", points[i].coordinates[j]);
            }else{
                printf("%d", points[i].coordinates[j]);
            }
        }
        printf(")\n");
    }

    // Compute the distance between all the points
    int temp_dmin, dmin_left, fmin_right = 0;
    for (int i = 0; i < num_points; i++){
        for (int j = 0; j < num_points; j++){

            if (points[i].coordinates[0] < 0 && points[j].coordinates[0]){
                // temp_dmin = distance(points[i], points[j]);
                printf("%d", temp_dmin);
            }
        }
    }
    
    printf("END OF PROGRAM");
    return 0;
}