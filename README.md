## <p align="center">High Performance Computing for Data Science - Project</p> 

This repository is a student project developed by Kevin Depedri and Davide Modolo for the "High Performance Computing for Data Science" course of the Master in Artificial Intelligent Systems at the University of Trento, a.y. 2022-2023. 

The repository is organized in the following way:
- In the folder `point generator`
- In the folder `sequential`
- In the folder `parallel`
- In the folder `runs` it is possible to access all the data relative to the runs performed using the Univeristy of Trento Cluster. These runs have been performed using 5 sizes of input point (see point generator), 4 node configurations and an increasing number of cores, from 1 to 80
- In the folder `data` it is possible to visualize the data of the runs inserted in tables and plotted as charts to allow a better understanging of the results

As writtem above, all the experiments have been ran on a HPC-cluster, but the code can also be tested on your own computer exploiting the cores and threads of your own CPU. This readme file presents all the steps to follow to correctly test the code on your machine.
****

# Install vcpkg
Vcpkg is a packet manager that allows to import the required libraries (OpenMPI) in windows. In this way when opening the code in your IDE you will be able to correctly parse the MPI_functions.
```bash
git clone https://github.com/Microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
vcpkg install mpi
vcpkg integrate install
```

# Install WSL with ubuntu
Windows Subsystem for Linux (WSL)
Install WSL from powershell:
```powershell
wsl --install
wsl ....
```
Run the following commands from your distro kernel
```bash
sudo apt-get update
sudo apt-get upgrade
sudo apt install mpich
```

# Run the code in WSL
Once that WSL has been correctly installed and updated with the necessary package, 
```bash
mpiexec -n <number_of_processes> mpi_<name_of_implementation> <path_to_points.txt> <flags>
```
Depending on the implementation there are different `flags` available, generally they are:
- `-v` : verbose enabled during mergesort part of the algorithm 
- `-e` : enumerate the pairs of point with smallest distance
- `-p` : print the pairs of point with smallest distance
