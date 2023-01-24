// project to find the closest pair of points in a k dimensional space sorting points

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

// utility functions to sort and merge
void merge(Point *points, int l, int m, int r, int dim){
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
    // allocate temp arrays
    Point *L = (Point *)malloc(n1 * sizeof(Point));
    Point *R = (Point *)malloc(n2 * sizeof(Point));
    for (i = 0; i < n1; i++){
        L[i] = points[l + i];
    }
    for (j = 0; j < n2; j++){
        R[j] = points[m + 1 + j];
    }
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2){
        if (L[i].coordinates[dim] <= R[j].coordinates[dim]){
            points[k] = L[i];
            i++;
        }
        else{
            points[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1){
        points[k] = L[i];
        i++;
        k++;
    }
    while (j < n2){
        points[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(Point *points, int l, int r, int dim){
    if (l < r){
        int m = l + (r - l) / 2;
        mergeSort(points, l, m, dim);
        mergeSort(points, m + 1, r, dim);
        merge(points, l, m, r, dim);
    }
}

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
    for (int i = 0; i < num_dimensions; i++){
        mergeSort(points, 0, num_points - 1, i);
    }

    // find the closest pair of points in the sorted array
    double min = closest(points, points, num_points);
    printf("The closest pair of points has distance %f\n", min);

    return 0;
}