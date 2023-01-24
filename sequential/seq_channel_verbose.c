#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils/util.h"

# define BOUND 100;


int main(int argc, char *argv[]){   
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

    // Compute the distance between all the points, points with x = 0 are considered on the left side
    int temp_dmin = 0;
    int dmin_left, dmin_right, dmin = BOUND;
    int comparisons = 0;
    for (int i = 0; i < num_points; i++){
        for (int j = i; j < num_points; j++){ // Use j = 0 to compute worst case
            if (i != j && points[i].coordinates[0] <= 0 && points[j].coordinates[0] <= 0){
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
            printf("Point %d (%d,%d) in left channel\n", i, points[i].coordinates[0], points[i].coordinates[1]);
            left_channel_points[num_left_channel_points] = points[i];
            num_left_channel_points += 1;
        }
        if (points[i].coordinates[0] > 0 && points[i].coordinates[0] < dmin){
            printf("Point %d (%d,%d) in right channel\n", i, points[i].coordinates[0], points[i].coordinates[1]);
            right_channel_points[num_right_channel_points] = points[i];
            num_right_channel_points += 1;
        }
    }
    printf("Number of points in the left/right of the channel: %d/%d\n", num_left_channel_points, num_right_channel_points);

    // Print the points of left channel
    for (int i = 0; i < num_left_channel_points; i++)
    {
        printf("Left point %d: (", i);
        for (int j = 0; j < num_dimensions; j++)
        {   
            if (j != num_dimensions-1){
                printf("%d, ", left_channel_points[i].coordinates[j]);
            }else{
                printf("%d", left_channel_points[i].coordinates[j]);
            }
        }
        printf(")\n");
    }
    // Print the points of right channel
    for (int i = 0; i < num_right_channel_points; i++)
    {
        printf("Right point %d: (", i);
        for (int j = 0; j < num_dimensions; j++)
        {   
            if (j != num_dimensions-1){
                printf("%d, ", right_channel_points[i].coordinates[j]);
            }else{
                printf("%d", right_channel_points[i].coordinates[j]);
            }
        }
        printf(")\n");
    }

    // Order the points in the channel
    Point *ordered_left_channel_points, *ordered_right_channel_points;
    ordered_left_channel_points = (Point *)malloc(num_left_channel_points * sizeof(Point));
    ordered_right_channel_points = (Point *)malloc(num_right_channel_points * sizeof(Point));
    // Reorder left channel points
    mergeSort(left_channel_points, num_left_channel_points, 0);
    mergeSort(right_channel_points, num_right_channel_points, 0);
    // Reorder right channel points

    // Compare left channel points with right channel points (only if their horizontal and vertical offset is < dmin)
    int dmin_channel = dmin;
    int channel_comparisons = 0;
    for (int i=0; i < num_left_channel_points; i++){
        printf("Cycle %d\n",i);
        for (int j=0; j < num_right_channel_points; j++){
            if (abs(left_channel_points[i].coordinates[0] - right_channel_points[j].coordinates[0]) < dmin && // To make it MORE EFFICIENT we can USE dmin_channel here
                abs(left_channel_points[i].coordinates[1] - right_channel_points[j].coordinates[1]) < dmin){  // In this way the comparisons in the channel will be less 
                temp_dmin = distance(left_channel_points[i], right_channel_points[j]);                        // and less as we fine lower numbers
                channel_comparisons += 1;
                printf("Comparison %d - Distance between left point %d and right point %d: %d\n", channel_comparisons, i, j, temp_dmin);
                if (temp_dmin < dmin_left){
                    dmin_channel = temp_dmin;
                    printf("New lowest distance\n");
                }
            } 
            else {
                printf("We can break here for point: %d\n", i);
                // break;
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
    free(ordered_left_channel_points);
    free(ordered_right_channel_points);
    
    printf("Memory free\n");
    return 0;
}