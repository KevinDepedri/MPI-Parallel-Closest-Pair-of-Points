echo "SENDING NORMAL JOBS"
cd normal
bash ./qsub_all_normal.sh

echo "SENDING PACKED JOBS"
cd ../packed
qsub ./qsub_all_packed.sh

echo "SENDING PACKED EXCLUSIVE JOBS"
cd ../packed_exclusive
qsub ./qsub_all_packed_excl.sh

echo "SENDING SCATTER JOBS"
cd ../scatter
qsub ./qsub_all_scatter.sh

echo "SENDING SCATTER EXCLUSIVE JOBS"
cd ../scatter_exclusive
qsub ./qsub_all_scatter_excl.sh

cd ..
echo "FINAL LIST OF ALL SENT JOBS"
qstat -u $USER
watch qstat -u $USER