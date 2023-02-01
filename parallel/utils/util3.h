typedef struct{
    int *coordinates;
    int num_dimensions;
} Point;

typedef struct{
    Point *points1;
    Point *points2;
    int num_pairs;
    double min_distance;
} Pairs;

extern double distance(Point, Point);

extern void merge(Point *, int, int, int, int);

extern void mergeSortRec(Point *, int, int, int);

extern void mergeSort(Point *, int, int);

extern void print_points(Point *, int, int);

extern void sendPointsPacked(Point*, int, int, int, MPI_Comm);

extern void recvPointsPacked(Point*, int, int, int, MPI_Comm);

extern double isMIN(double, double);

extern double isMINof3(double, double, double);

extern void recSplit(Point*, int, Pairs*);

extern Point *parallel_mergesort(Point *, char[], int, int, int);