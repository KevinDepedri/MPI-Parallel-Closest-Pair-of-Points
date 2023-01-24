#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Structure for a point in any number of dimensions
typedef struct Point{
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

    FILE* fp = fopen("points_channel.txt", "r");
    if (fp == NULL)
    {
        perror("Error opening file\n");
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
    int temp_dmin = 0;
    int dmin_left, dmin_right = 100;
    int comparisons = 0;
    for (int i = 0; i < num_points; i++){
        for (int j = i; j < num_points; j++){ // Use j = 0 to compute worst case
            if (i != j && points[i].coordinates[0] < 0 && points[j].coordinates[0] < 0){
                temp_dmin = distance(points[i], points[j]);
                if (temp_dmin < dmin_left){
                    dmin_left = temp_dmin;
                }
                comparisons += 1;
                printf("Comparison %d - Left distance between point %d and %d: %d\n", comparisons, i, j, temp_dmin);
            }
            if (i != j && points[i].coordinates[0] > 0 && points[j].coordinates[0] > 0){
                temp_dmin = distance(points[i], points[j]);
                if (temp_dmin < dmin_right){
                    dmin_right = temp_dmin;
                }
                comparisons += 1;
                printf("Comparison %d - Right distance between point %d and %d: %d\n", comparisons, i, j, temp_dmin);
            }
        }
    }
    printf("Lower distance left side: %d\n", dmin_left);
    printf("Lower distance right side: %d\n", dmin_right);

    for (int i =0; i< num_points ;i++){
        free(points[i].coordinates);
    }
    free(points);
    return 0;
}