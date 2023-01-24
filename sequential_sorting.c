// project to find the closest pair of points in a k dimensional space sorting points

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils/util.h"

// Structure for a point in any number of dimensions


// utility function to find the minimum of two doubles
double isMinor(double x, double y){
    return x < y ? x : y;
}

// utility function to find the smallest distance in a strip
double stripClosest(Point *strip, int size, double d){
    double min = d;
    for (int i = 0; i < size; i++){
        for (int j = i + 1; j < size && (strip[j].coordinates[1] - strip[i].coordinates[1]) < min; j++){
            if (distance(strip[i], strip[j]) < min){
                min = distance(strip[i], strip[j]);
            }
        }
    }
    return min;
}

// recursive function to find the smallest distance
double closest(Point *points1, Point *points2, int n){
    if (n <= 3){
        double min = distance(points1[0], points2[0]);
        for (int i = 0; i < n; i++){
            for (int j = i+1; j < n; j++){
                if (distance(points1[i], points2[j]) < min){
                    min = distance(points1[i], points2[j]);
                }
            }
        }
        return min;
    }
    int mid = n / 2;
    Point midPoint = points1[mid];
    Point *points2l = (Point *)malloc(mid * sizeof(Point));
    Point *points2r = (Point *)malloc((n - mid) * sizeof(Point));
    int li = 0, ri = 0;
    for (int i = 0; i < n; i++){
        if (points2[i].coordinates[0] <= midPoint.coordinates[0]){
            points2l[li] = points2[i];
            li++;
        }
        else{
            points2r[ri] = points2[i];
            ri++;
        }
    }
    double dl = closest(points1, points2l, mid);
    double dr = closest(points1 + mid, points2r, n - mid);
    double d = isMinor(dl, dr);
    Point *strip = (Point *)malloc(n * sizeof(Point));
    int j = 0;
    for (int i = 0; i < n; i++){
        if (abs(points2[i].coordinates[0] - midPoint.coordinates[0]) < d){
            strip[j] = points2[i];
            j++;
        }
    }
    // free memory
    free(points2l);
    free(points2r);

    return stripClosest(strip, j, d);
}



int main(){
    Point* points;
    int num_points, num_dimensions;
    FILE* fp = fopen("points_bruteforce.txt", "r");
    if (fp == NULL){
        perror("Error opening file");
        return 1;
    }
    // Read the number of points and dimensions from the first line of the file
    fscanf(fp, "%d %d", &num_points, &num_dimensions);
    // Allocate memory for the points
    points = (Point *)malloc(num_points * sizeof(Point));
    for (int i = 0; i < num_points; i++){
        points[i].num_dimensions = num_dimensions;
        points[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
        for (int j = 0; j < num_dimensions; j++){
            fscanf(fp, "%d", &points[i].coordinates[j]);
        }
    }
    fclose(fp);
    printf("Loaded %d points in %d dimensions\n", num_points, num_dimensions);

    // sort points by each dimension
    mergeSort(points, num_points - 1, 0);
    // print sorted points
    for (int i = 0; i < num_points; i++){
        for (int j = 0; j < num_dimensions; j++){
            printf("%d ", points[i].coordinates[j]);
        }
        printf("\n"); 
    }

    // find the closest pair of points in the sorted array
    int n = num_points/num_dimensions;
    // create Px and Py
    Point *Px = (Point *)malloc(n * sizeof(Point));
    Point *Py = (Point *)malloc(n * sizeof(Point));
    for (int i = 0; i < n; i++){
        Px[i] = points[i];
        Py[i] = points[i + n];
    }

    double min = closest(Px, Py, n);
    printf("The closest pair of points has distance %f\n", min);

    return 0;
}