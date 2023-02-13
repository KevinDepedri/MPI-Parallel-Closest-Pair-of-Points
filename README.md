## <p align="center">High Performance Computing for Data Science - Project</p> 

This repository is a student project developed by Kevin Depedri and Davide Modolo for the "High Performance Computing for Data Science" course of the Master in Artificial Intelligent Systems at the University of Trento, a.y. 2022-2023. 

The repository is organized in the following way:
- In the folder `point generator` the smallest set of input point are provided, together with the python codes to generate bigger set of input points
- In the folder `sequential` the sequential implementation of the Bruteforce approach and of the Divide et Impera approach are provided, together with their executable files
- In the folder `parallel` the parallel implementation of the Bruteforce approach and of the Divide et Impera approach are provided, together with their executable files (they requires the command `mpiexec` from the library `mpitch` to be run, see more below). In particular, for the Divide et Impera approach two implementations can be found:
  - `parallel_recursive_short`, it shows the logic behind the algorithm from a higher level, abstracting much of what is happening in the Merge Sort and in the Divide et Impera parts of the algorithm
  - `parallel_recursive_long`, it shows all the logic behind all the algorithm in a single file, useful to understand in an easier way the communications between processes
- In the folder `runs` it is possible to access all the data relative to the runs performed using the Univeristy of Trento HPC-Cluster. These runs have been performed using 5 sizes of input point (see point generator), 4 node configurations and an increasing number of cores, from 1 to 80
- In the folder `data` it is possible to visualize the data of the runs inserted in tables and plotted as charts to allow a better understanging of the results

As writtem above, all the experiments have been ran on a HPC-cluster, but the code can also be tested on your own computer exploiting the cores and threads of your CPU. This readme file presents all the steps to follow to correctly test the code on your machine.
****

# Install Vcpkg
Vcpkg is a packet manager that allows to easily import the required libraries (MPICH) in Windows. In this way when opening the code in your IDE you will be able to correctly parse the MPI_functions. If you are a linux user then skip directly to the second part of the next section.
```bash
git clone https://github.com/Microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
vcpkg install mpi
vcpkg integrate install
```

# Install WSL with Ubuntu
Windows Subsystem for Linux (WSL) allows to install a linux kernerl directly in Windows. This is the quickest way to have access to MPICH in windows. To install WSL with Ubuntu follow the ensuing commands in Windows Powershell
Install WSL from powershell:
```powershell
wsl --install
wsl --install -d Ubuntu
```
Once Ubuntu has been installed, run the following instruction in the Ubuntu Kernel
```bash
sudo apt-get update
sudo apt-get upgrade
sudo apt install mpich
```

# Run the code in WSL
Once that Ubuntu has been correctly installed and updated with the necessary package, it will now be possible to run the code using the following commands.
- If you would like to use a number of processes n, up to the number of cores physically avaiable in your cpu then use the flowing command
  ```bash
  mpiexec -n <number_of_processes> mpi_<name_of_implementation> <path_to_points.txt> <flags>
  ```
- If you would like to use a bigger of processes n, up to the number of threads physically avaiable in your cpu then use the flowing command
  ```bash
  mpiexec -use-hwthread-cpus -n <number_of_processes> mpi_<name_of_implementation> <path_to_points.txt> <flags>
  ```
- If you would like to use an arbitrary number of processes n, then use the flowing command
  ```bash
  mpiexec -oversubscribe -n <number_of_processes> mpi_<name_of_implementation> <path_to_points.txt> <flags>
  ```

Using any of the previous commands, and depending on the implementation, there are different `flags` available, generally they are:
- `-v` : verbose enabled during Mergesort part of the algorithm, use it only with small set of point like 1H2d (100 points) to understand the process performed in the Mergesort part of the algorith
- `-e` : enumerate the pairs of point with smallest distance
- `-p` : print the pairs of point with smallest distance

