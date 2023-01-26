#ifdef _OPENMP
# include <omp.h>
#endif
#include<stdlib.h>
#include<stdio.h>

// To compile in console use one of the follwing
// gcc -g -Wall -fopenmp -o omp_hello_world hello_world.c
// To run
// ./omp_hello_world

void Hello(void);

int main(int argc,char**argv){

 #pragma omp parallel num_threads(4)
 Hello();

 return 0;
}

void Hello(void) {
# ifdef _OPENMP
    int my_rank = omp_get_thread_num ( );
    int thread_count = omp_get_num_threads ( );
# else
    int my_rank = 0;
    int thread_count = 1;
# endif
 printf("Hello from thread %d of %d\n", my_rank, thread_count);
 }