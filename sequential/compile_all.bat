@REM File to gcc compile sequential_XXX.c and utils/util.c together
gcc -o seq_channel_verbose .\seq_channel_verbose.c utils/util.c
gcc -o sequential_channel .\sequential_channel.c utils/util.c
gcc -o sequential_bruteforce .\sequential_bruteforce.c utils/util.c