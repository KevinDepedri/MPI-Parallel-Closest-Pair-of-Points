echo "SENDING PACKED JOBS"
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/packed/start_parallel_recursive_packed_2cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/packed/start_parallel_recursive_packed_4cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/packed/start_parallel_recursive_packed_8cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/packed/start_parallel_recursive_packed_16cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/packed/start_parallel_recursive_packed_24cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/packed/start_parallel_recursive_packed_32cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/packed/start_parallel_recursive_packed_48cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/packed/start_parallel_recursive_packed_64cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/packed/start_parallel_recursive_packed_80cores.sh

echo "LIST OF ALL SENT JOBS"
qstat -u $USER