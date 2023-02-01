typedef struct{
    int *coordinates;
    int num_dimensions;
} Point;

extern double distance(Point, Point);

extern void merge(Point *, int, int, int, int);

extern void mergeSortRec(Point *, int, int, int);

extern void mergeSort(Point *, int, int);

extern void print_points(Point *, int, int);

extern void printPoint(Point);

extern void sendPointsPacked(Point*, int, int, int, MPI_Comm);

extern void recvPointsPacked(Point*, int, int, int, MPI_Comm);

extern double isMIN(double, double);

extern double isMINof3(double, double, double);

extern double sequential_closestpair_recursive(Point*, int );


extern double parallel_closestpair(Point *, int, int, int, int, int, int);