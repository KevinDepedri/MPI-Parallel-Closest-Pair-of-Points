echo 'COMPILE'
mpicc -std=c99 -g -Wall -o mpi_recursive3 parallel_recursive3.c utils/util.c -lm
echo 'PARALLEL RUN 6 CORES'
mpiexec -n 6 mpi_recursive3
echo 'SEQUENTIAL RUN'
mpiexec -n 2 mpi_recursive3