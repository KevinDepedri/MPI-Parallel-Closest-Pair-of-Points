echo "SENDING SCATTER JOBS"
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter/start_parallel_recursive_scatter_1core.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter/start_parallel_recursive_scatter_2cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter/start_parallel_recursive_scatter_4cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter/start_parallel_recursive_scatter_8cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter/start_parallel_recursive_scatter_16cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter/start_parallel_recursive_scatter_24cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter/start_parallel_recursive_scatter_32cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter/start_parallel_recursive_scatter_48cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter/start_parallel_recursive_scatter_64cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter/start_parallel_recursive_scatter_80cores.sh

echo "LIST OF ALL SENT JOBS"
qstat -u $USER