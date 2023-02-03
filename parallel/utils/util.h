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

extern int differPoint(Point, Point);

extern void merge(Point *, int, int, int, int);

extern void mergeSortRec(Point *, int, int, int);

extern void mergeSort(Point *, int, int);

extern void print_points(Point *, int, int);

extern void sendPointsPacked(Point*, int, int, int, MPI_Comm);

extern void recvPointsPacked(Point*, int, int, int, MPI_Comm);

extern double isMIN(double, double);

extern double isMINof3(double, double, double);

extern void printPoint(Point);

// ------------------------------------------------------
extern void recSplit(Point*, int, Pairs*, int); //TO REMOVE SOON
extern double sequential_closestpair_recursive(Point*, int );
// ------------------------------------------------------

extern Point *parallel_mergesort(Point *, char[], int, int, int);

// ------------------------------------------------------
extern double parallel_closestpair(Point *, int, int, int, int, int, int); //TO REMOVE SOON
extern void parallel_closestpair2(Point *, int, int, int, int, int, int, int, int);
// ------------------------------------------------------

extern void getUniquePairs(Pairs*, double, int, int, int, int);