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
        points[i].num_dimensions = num_dimensions;
        points[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
        for (int j = 0; j < num_dimensions; j++)
        {
            fscanf(fp, "%d", &points[i].coordinates[j]);
        }
    }
    fclose(fp);
    printf("The number of points is %d\n", num_points);
    int p1 = 0, p2 = 1;
    int count = 0;
    double min_distance = distance(points[0], points[1]);
    for (int i = 0; i < num_points; i++){
        for (int j = i + 1; j < num_points; j++){
            double dist = distance(points[i], points[j]);
            if (dist < min_distance){
                p1 = i;
                p2 = j;
                min_distance = dist;
                count = 1;
            }
            else if (dist == min_distance){
                count++;
            }
        }
    }
    printf("The minimum distance is %f with points #%d and #%d. ", min_distance, p1, p2);
    printf("There are %d pairs with this distance.\n", count);
    printf("The first point coordinates are:\n");
    for (int i = 0; i < num_dimensions; i++){
        printf("%d ", points[p1].coordinates[i]);
    }
    printf("\nThe second point coordinates are:\n");
    for (int i = 0; i < num_dimensions; i++){
        printf("%d ", points[p2].coordinates[i]);
    }

    // free the memory
    free(points);
 
    return 0;
}