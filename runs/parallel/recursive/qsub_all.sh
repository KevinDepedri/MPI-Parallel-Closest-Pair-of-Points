echo "SENDING NORMAL JOBS"
bash /home/kevin.depedri/hpc3/runs/parallel/recursive/normal/qsub_all_normal.sh

echo "SENDING PACKED JOBS"

echo "SENDING PACKED EXCLUSIVE JOBS"

echo "SENDING SCATTER JOBS"

echo "SENDING SCATTER EXCLUSIVE JOBS"

echo "LIST OF ALL SENT JOBS"
qstat -u $USER
watch qstat -u $USER