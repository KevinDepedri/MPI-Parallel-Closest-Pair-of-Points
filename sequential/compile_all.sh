# File to gcc compile sequential_XXX.c and utils/util.c together
echo 'COMPILE BRUTEFORCE'
gcc -std=c99 -g -Wall -o sequential_bruteforce sequential_bruteforce.c utils/util.c -lm
echo 'COMPILE RECURSIVE'
gcc -std=c99 -g -Wall -o sequential_recursive sequential_recursive.c utils/util.c -lm