typedef struct{
    int *coordinates;
    int num_dimensions;
} Point;

extern double distance(Point, Point );

extern void merge(Point *, int, int, int, int);

extern void mergeSortRec(Point *, int, int, int);

extern void mergeSort(Point *, int, int);

extern void print_points(Point *point_list, int num_points, int rank_process);

extern void sendPointsPacked(Point* points, int numPoints, int destination, int tag, MPI_Comm comm);

extern void recvPointsPacked(Point* points, int numPoints, int source, int tag, MPI_Comm comm);