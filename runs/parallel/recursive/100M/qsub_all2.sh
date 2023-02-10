echo "SENDING SCATTER JOBS"
cd scatter
bash ./qsub_all_scatter.sh

echo "SENDING SCATTER EXCLUSIVE JOBS"
cd ../scatter_exclusive
bash ./qsub_all_scatter_excl.sh

cd ..
echo "FINAL LIST OF ALL SENT JOBS"
qstat -u $USER
watch qstat -u $USER