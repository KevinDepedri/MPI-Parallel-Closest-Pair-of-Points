# Path for the points "/home/kevin.depedri/points/250M5d.txt"

echo "SENDING NORMAL JOBS"
qsub /home/kevin.depedri/hpc3/runs/parallel/bruteforce/normal/start_parallel_bruteforce_normal_2cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/bruteforce/normal/start_parallel_bruteforce_normal_4cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/bruteforce/normal/start_parallel_bruteforce_normal_8cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/bruteforce/normal/start_parallel_bruteforce_normal_16cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/bruteforce/normal/start_parallel_bruteforce_normal_32cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/bruteforce/normal/start_parallel_bruteforce_normal_64cores.sh

echo "LIST OF ALL SENT JOBS"
qstat -u $USER