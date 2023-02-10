echo "SENDING PACKED JOBS"
cd packed
bash ./qsub_all_packed.sh

echo "SENDING PACKED EXCLUSIVE JOBS"
cd ../packed_exclusive
bash ./qsub_all_packed_excl.sh

cd ..
echo "FINAL LIST OF ALL SENT JOBS"
qstat -u $USER
watch qstat -u $USER