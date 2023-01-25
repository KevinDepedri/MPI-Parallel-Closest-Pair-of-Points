#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils/util.h"
#include <time.h>
# define BOUND 100;


int main(int argc, char *argv[]){
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    Point *points;
    int num_points, num_dimensions;

    FILE* fp = fopen("../point_generator/points.txt", "r");
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
    printf("%d", num_points);

    // Compute the distance between all the points, points with x = 0 are considered on the left side
    int temp_dmin = 0;
    int dmin_left, dmin_right, dmin = BOUND;
    int comparisons = 0;
    for (int i = 0; i < num_points; i++){
        for (int j = i; j < num_points; j++){ // IMPROVEMENT 1: Use j = 1 instead of j = 0
            if (i != j && points[i].coordinates[0] <= 0 && points[j].coordinates[0] <= 0){
                temp_dmin = distance(points[i], points[j]);
                if (temp_dmin < dmin_left){
                    dmin_left = temp_dmin;
                }
                comparisons += 1;
            }
            if (i != j && points[i].coordinates[0] > 0 && points[j].coordinates[0] > 0){
                temp_dmin = distance(points[i], points[j]);
                if (temp_dmin < dmin_right){
                    dmin_right = temp_dmin;
                }
                comparisons += 1;
            }
        }
    }
    dmin = dmin_left;
    if (dmin_right < dmin){
        dmin = dmin_right;
    }
    printf("Lower distance left side: %d\n", dmin_left);
    printf("Lower distance right side: %d\n", dmin_right);
    printf("Global lower distance: %d\n", dmin);

    // Work in the channel
    Point *left_channel_points, *right_channel_points;
    int num_left_channel_points = 0;
    int num_right_channel_points = 0;
    left_channel_points = (Point *)malloc(num_points/2 * sizeof(Point));
    right_channel_points = (Point *)malloc(num_points/2 * sizeof(Point));
    for (int i=0; i < num_points; i++){
        if (points[i].coordinates[0] > -dmin && points[i].coordinates[0] <= 0){
            left_channel_points[num_left_channel_points] = points[i];
            num_left_channel_points += 1;
        }
        if (points[i].coordinates[0] > 0 && points[i].coordinates[0] < dmin){
            right_channel_points[num_right_channel_points] = points[i];
            num_right_channel_points += 1;
        }
    }
    printf("Number of points in the left/right of the channel: %d/%d\n", num_left_channel_points, num_right_channel_points);


    // Order the points in the channel
    // Point *ordered_left_channel_points, *ordered_right_channel_points;
    // ordered_left_channel_points = (Point *)malloc(num_left_channel_points * sizeof(Point));
    // ordered_right_channel_points = (Point *)malloc(num_right_channel_points * sizeof(Point));
    // Reorder left channel points
    mergeSort(left_channel_points, num_left_channel_points, 0);
    mergeSort(right_channel_points, num_right_channel_points, 0);
    // Reorder right channel points

    // Compare left channel points with right channel points (only if their horizontal and vertical offset is < dmin)
    int dmin_channel = dmin;
    int channel_comparisons = 0;
    for (int i=0; i < num_left_channel_points; i++){
        for (int j=0; j < num_right_channel_points; j++){
            if (abs(left_channel_points[i].coordinates[0] - right_channel_points[j].coordinates[0]) < dmin_channel && // IMPROVEMENT 2. Using dmin_channel instead of dmin
                abs(left_channel_points[i].coordinates[1] - right_channel_points[j].coordinates[1]) < dmin_channel){  // In this way the comparisons in the channel will be less 
                temp_dmin = distance(left_channel_points[i], right_channel_points[j]);                        // and less as we fine lower numbers
                channel_comparisons += 1;
                if (temp_dmin < dmin_left){
                    dmin_channel = temp_dmin;
                }
            } 
            else {
                break;
            }
        }
    }
    printf("Global lower distance: %d\n", dmin_channel);

    // Free memory for initial points list
    for (int i =0; i< num_points ;i++){
        free(points[i].coordinates);
    }
    free(points);
    free(left_channel_points);
    free(right_channel_points);
    // free(ordered_left_channel_points);
    // free(ordered_right_channel_points);
    
    printf("Memory free\n");
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time elapsed: %f\n", cpu_time_used);
    // wait for a key press
    getchar();
    return 0;
}