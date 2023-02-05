# Path for the points "/home/kevin.depedri/points/250M5d.txt"

echo "SENDING NORMAL JOBS"
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/normal/start_parallel_recursive_normal_2cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/normal/start_parallel_recursive_normal_4cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/normal/start_parallel_recursive_normal_8cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/normal/start_parallel_recursive_normal_16cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/normal/start_parallel_recursive_normal_32cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/normal/start_parallel_recursive_normal_64cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/normal/start_parallel_recursive_normal_80cores.sh
# qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/normal/start_parallel_recursive_normal_85cores.sh

echo "LIST OF ALL SENT JOBS"
qstat -u $USER