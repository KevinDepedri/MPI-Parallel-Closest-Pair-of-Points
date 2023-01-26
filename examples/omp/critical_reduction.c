#ifdef _OPENMP
# include <omp.h>
#endif
#include<stdlib.h>
#include<stdio.h>

#define SIZE 10

// To compile in console
// gcc -g -Wall -fopenmp -o omp_pi pi.c
// To run in the start file
// ./omp_pi

void Sum(int input_array[], int* index, int* total);

int main(int argc,char**argv){
 int array[SIZE];
 int total_sum=0;
 int i=1;
 
 int index=0;
 for(index=0; index<SIZE; index++){
    array[index] = index;
 }
 
 #pragma omp parallel num_threads(SIZE)
 Sum(array, &i, &total_sum);

 printf("Final result: %d", total_sum);
 return 0;
}

void Sum(int input_array[], int* index, int* total){
 int input_value = input_array[*index];
 int total_pre_sum = *total;
 #pragma omp critical
 {
 *total += input_value;
 *index += 1;
 }
 printf("Input value: %d Total pre-sum: %d Total post-sum: %d \n", input_value, total_pre_sum, *total);
 }