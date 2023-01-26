@REM File to gcc compile sequential_XXX.c and utils/util.c together
gcc -o sequential_bruteforce .\sequential_bruteforce.c utils/util.c
gcc -o sequential_recursive .\sequential_recursive.c utils/util.c