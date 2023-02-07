echo "SENDING SCATTER EXCLUSIVE JOBS"
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter_exclusive/start_parallel_recursive_scatter_excl_2cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter_exclusive/start_parallel_recursive_scatter_excl_4cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter_exclusive/start_parallel_recursive_scatter_excl_8cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter_exclusive/start_parallel_recursive_scatter_excl_16cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter_exclusive/start_parallel_recursive_scatter_excl_24cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter_exclusive/start_parallel_recursive_scatter_excl_32cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter_exclusive/start_parallel_recursive_scatter_excl_48cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter_exclusive/start_parallel_recursive_scatter_excl_64cores.sh
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/scatter_exclusive/start_parallel_recursive_scatter_excl_80cores.sh

echo "LIST OF ALL SENT JOBS"
qstat -u $USER