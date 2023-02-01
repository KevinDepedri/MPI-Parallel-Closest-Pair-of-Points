echo 'COMPILE'
mpicc -std=c99 -g -Wall -o mpi_recursive2 parallel_recursive2.c utils/util.c -lm
echo 'PARALLEL RUN 6 CORES'
mpiexec -n 6 mpi_recursive2
# echo 'PARALLEL RUN 4 CORES'
# mpiexec -n 4 mpi_recursive2
echo 'SEQUENTIAL RUN'
mpiexec -n 2 mpi_recursive2