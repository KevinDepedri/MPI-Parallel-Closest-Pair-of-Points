echo "SENDING NORMAL JOBS"
bash /home/kevin.depedri/hpc3/runs/parallel/recursive/normal/qsub_all_normal.sh

echo "SENDING PACKED JOBS"
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/packed/qsub_all_packed.sh

echo "SENDING PACKED EXCLUSIVE JOBS"
qsub /home/kevin.depedri/hpc3/runs/parallel/recursive/packed_exclusive/qsub_all_packed_excl.sh

echo "SENDING SCATTER JOBS"


echo "SENDING SCATTER EXCLUSIVE JOBS"


echo "FINAL LIST OF ALL SENT JOBS"
qstat -u $USER
watch qstat -u $USER