#include <math.h>
#include <stdlib.h>
#include "util.h"

double distance(Point p1, Point p2){
    double sum = 0;
    int n = p1.num_dimensions;
    for (int i = 0; i < n; i++){
        sum += pow((p1.coordinates[i] - p2.coordinates[i]), 2);
    }
    return sqrt(sum);
}

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

    free(L);
    free(R);
}

void mergeSortRec(Point *points, int l, int r, int dim){
    if (l < r){
        int m = l + (r - l) / 2;
        mergeSortRec(points, l, m, dim);
        mergeSortRec(points, m + 1, r, dim);
        merge(points, l, m, r, dim);
    }
}

void mergeSort(Point *points, int dim, int axis){
    mergeSortRec(points, 0, dim - 1, axis);
}