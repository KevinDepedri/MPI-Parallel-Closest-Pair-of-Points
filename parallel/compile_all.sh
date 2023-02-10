# File to mpicc compile parallel_recursive_XXX.c and utils/util.c together
echo 'COMPILE LONG VERSION'
mpicc -std=c99 -g -Wall -o mpi_recursive_long parallel_recursive_long.c utils/util.c -lm
echo 'COMPILE SHORT VERSION'
mpicc -std=c99 -g -Wall -o mpi_recursive_short parallel_recursive_short.c utils/util.c -lm
echo 'COMPILE BRUTEFORCE VERSION'
mpicc -std=c99 -g -Wall -o mpi_bruteforce parallel_bruteforce.c utils/util.c -lm
echo 'COMPILE 1-2 CORE VERSION'
mpicc -std=c99 -g -Wall -o mpi_recursive_long_2cores parallel_recursive_long_2cores.c utils/util.c -lm