#ifndef UTIL_H
typedef struct{
    int *coordinates;
    int num_dimensions;
} Point;

extern double distance(Point, Point );

extern void merge(Point *, int, int, int, int);

extern void mergeSortRec(Point *, int, int, int);

extern void mergeSort(Point *, int, int);

#endif