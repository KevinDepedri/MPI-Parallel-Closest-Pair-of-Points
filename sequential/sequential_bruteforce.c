#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils/util.h"
#include <time.h>


int main(int argc, char *argv[]){
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    Point *points;
    int num_points, num_dimensions;
    if(argc < 2){
        perror("Error: no file name or path has been provided as first argument (points input file)\n");
        return -1;
    }

    int ENUMERATE_PAIRS_OF_POINTS = 0, PRINT_PAIRS_OF_POINTS = 0, INVALID_FLAG = 0;
    for (size_t option_id = 2; option_id < argc; option_id++){
        switch (argv[option_id][1]){
            case 'e': ENUMERATE_PAIRS_OF_POINTS = 1; break;
            case 'p': PRINT_PAIRS_OF_POINTS = 1; break;
            default: INVALID_FLAG = 1; break;
        }
    }
    if (INVALID_FLAG == 1){
        perror("ERROR: the only valid flag arguments are:\n \t-e : enumerate the pairs of point with smallest distance\n \t-p : print the pairs of point with smallest distance\n");
        return -1;
    }

    // Open input point file on master process
    FILE *point_file = fopen(argv[1], "r");
    if (point_file == NULL){
        perror("Error opening file\n");
        return 1;
    }

    // Read the number of points and dimensions from the first line of the file
    fscanf(point_file, "%d %d", &num_points, &num_dimensions);
    if(num_points < 2){
        printf("Error: the number of points must be greater than 1");
        return 1;
    }

    // Allocate memory for the points
    points = (Point *)malloc(num_points * sizeof(Point));
    for (int i = 0; i < num_points; i++){
        points[i].num_dimensions = num_dimensions;
        points[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));

        for (int j = 0; j < num_dimensions; j++){
            fscanf(point_file, "%d", &points[i].coordinates[j]);
        }
    }
    fclose(point_file);

    // printf("Loaded %d points in %d dimensions\n", num_points, num_dimensions);
    // count and save all points indeces with minimum distance
    int *min_points1 = (int *)malloc(num_points * sizeof(int));
    int *min_points2 = (int *)malloc(num_points * sizeof(int));
    int points1_count = 0;
    int points2_count = 0;
    int count = 0;

    // Initialize the minimum distance to the distance between the first two points
    double min_distance = distance(points[0], points[1]);

    // Find the minimum distance between any two points
    for (int i = 0; i < num_points; i++){
        for (int j = i + 1; j < num_points; j++){
            double dist = distance(points[i], points[j]);
            // if distance is less than min_distance, update min_distance and reset counters
            if (dist < min_distance){
                min_distance = dist;
                
                min_points1[0] = i;
                min_points2[0] = j;
                count = 1;

                points1_count = 1;
                points2_count = 1;

            }
            // if distance is equal to min_distance, add the pair to the list
            else if (dist == min_distance){
                min_points1[points1_count++] = i;
                min_points2[points2_count++] = j;
                count++;
            }
        }
    }
    // print results
    printf("The minimum distance is %f.\n", min_distance);

    if (ENUMERATE_PAIRS_OF_POINTS == 1)
        printf("There are %d pairs with this distance.\n", count);
    
    // for each point in min_points1 print point1 and point2 /n
    if (PRINT_PAIRS_OF_POINTS == 1){
    for (int i = 0; i < points1_count; i++){
        printf("Point #%d: ", min_points1[i]);
        for (int j = 0; j < num_dimensions; j++){
            printf("%d ", points[min_points1[i]].coordinates[j]);
        }
        printf("Point #%d: ", min_points2[i]);
        for (int j = 0; j < num_dimensions; j++){
            printf("%d ", points[min_points2[i]].coordinates[j]);
        }
        printf("\n");
    }
    }
    

    // free the memory
    free(points);
    free(min_points1);
    free(min_points2);

    printf("Memory free\n");
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time elapsed: %f\n", cpu_time_used);
    // wait for a key press
    getchar();
 
    return 0;
}