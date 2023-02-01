echo 'COMPILE'
mpicc -std=c99 -g -Wall -o mpi_recursive2 parallel_recursive2.c utils/*.c -lm
echo 'PARALLEL RUN'
mpiexec -n 6 mpi_recursive2
echo 'SEQUENTIAL RUN'
mpiexec -n 2 mpi_recursive2