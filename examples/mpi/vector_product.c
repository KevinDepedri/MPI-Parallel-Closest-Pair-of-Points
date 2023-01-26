#include<mpi.h>
#include<stdlib.h>
#include<stdio.h>

#define ROWS 8
#define COLUMNS 2

// To compile in console
// mpicc -std=c99 -g -Wall -o script script.c -lm

int main(int argc,char**argv){
 MPI_Init(NULL,NULL);

 int world_size;
 MPI_Comm_size(MPI_COMM_WORLD,&world_size);

 int world_rank;
 MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
 
 //Define variables to receive transferred values
 int flattened_matrix [ROWS*COLUMNS];
 int received_scatter[COLUMNS];
 int received_vector [COLUMNS];
 int products[COLUMNS];
 int flattened_output [ROWS*COLUMNS];
 int total = 0;

 if (world_rank==0){
    // Define input matrix and input vector
    int input_matrix [ROWS][COLUMNS];
    int input_vector [COLUMNS];

    // Define output matrix
    int output_matrix [ROWS][COLUMNS];

    // Populate randomly the matrix and the vector
    printf("Generated input matrix:\n");
    for (int row=0; row<ROWS; row++){
        for (int column=0; column<COLUMNS; column++){
            input_matrix[row][column] = rand() % 100 + 1;
            printf("%d ", input_matrix[row][column]);
        }
        printf("\n");
    }

    printf("\nGenerated input vector:\n");
    for (int column=0; column<COLUMNS; column++){
        input_vector[column] = rand() % 100 + 1;
        printf("%d ", input_vector[column]);
    }
    printf("\n");

    // Flatten the matrix to scatter it
    printf("\nFlattened matrix:\n");
    int position = 0;
    for (int row=0; row<ROWS; row++){
        for (int column=0; column<COLUMNS; column++){
            flattened_matrix[position] = input_matrix[row][column];
            printf("%d ", flattened_matrix[position]);
            position++;
        }
    }
    printf("\n\n");

    // Scatter each row of the matrix on a different process
    MPI_Scatter(&flattened_matrix,COLUMNS,MPI_INT,
                &received_scatter,COLUMNS,MPI_INT,0,MPI_COMM_WORLD);
    
    // Broadcast the vector to each process
    MPI_Bcast(&input_vector,COLUMNS,MPI_INT,0,MPI_COMM_WORLD);

    // Compute products
    for (int i=0; i<COLUMNS; i++){
        products[i] = received_scatter[i] * input_vector[i];
    }

    // Gather the data
    MPI_Gather(&products,COLUMNS,MPI_INT,&flattened_output,COLUMNS,MPI_INT,0,MPI_COMM_WORLD);

    // Un-flatten the matrix
    printf("\nOutput matrix:\n");
    position = 0;
    for (int row=0; row<ROWS; row++){
        for (int column=0; column<COLUMNS; column++){
            output_matrix[row][column] = flattened_output[position];
            printf("%d ", output_matrix[row][column]);
            position++;
        }
        printf("\n");
    }
    printf("\n");
    
    // Reduce to compute the total
    int subtotal = products[0] + products[1];
    MPI_Reduce(&subtotal,&total,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    printf("Result of the sum of the products: %d",total);

 }
else if(world_rank!=0){
    // Receive the scatter
    MPI_Scatter(&flattened_matrix,COLUMNS,MPI_INT,
                &received_scatter,COLUMNS,MPI_INT,0,MPI_COMM_WORLD);

    // Reveive the broadcasr
    MPI_Bcast(&received_vector,COLUMNS,MPI_INT,0,MPI_COMM_WORLD);

    // Print results
    printf("Process %d received scatter: %d %d and vector: %d %d \n",world_rank, received_scatter[0], received_scatter[1], received_vector[0], received_vector[1]);

    // Compute products
    for (int i=0; i<COLUMNS; i++){
        products[i] = received_scatter[i] * received_vector[i];
    }
    
    // Gather the data
    MPI_Gather(&products,COLUMNS,MPI_INT,&flattened_output,COLUMNS,MPI_INT,0,MPI_COMM_WORLD);

    // Reduce to compute the total
    int subtotal = products[0] + products[1];
    MPI_Reduce(&subtotal,&total,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
} 

 MPI_Finalize();
 return 0;
}