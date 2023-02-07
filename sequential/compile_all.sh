# File to gcc compile sequential_XXX.c and utils/util.c together. 
gcc -std=c99 -g -Wall -o sequential_bruteforce sequential_bruteforce.c utils/util.c -lm
gcc -std=c99 -g -Wall -o sequential_recursive sequential_recursive.c utils/util.c -lm
# WARNING: if executing this files leads to error try the commands one at time