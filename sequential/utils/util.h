typedef struct{
    int *coordinates;
    int num_dimensions;
} Point;

extern double distance(Point, Point );

extern void merge(Point *, int, int, int, int);

extern void mergeSortRec(Point *, int, int, int);

extern void mergeSort(Point *, int, int);

extern double isMIN(double, double);

extern double isMINof3(double, double, double);

extern double recSplit(Point*, int);