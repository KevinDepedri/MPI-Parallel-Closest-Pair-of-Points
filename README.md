## <p align="center">High Performance Computing for Data Science - Project</p> 

This repository is a student project developed by Kevin Depedri and Davide Modolo for the "High Performance Computing for Data Science" course of the Master in Artificial Intelligent Systems at the University of Trento, a.y. 2022-2023. 

This readme file presents all the steps to follow for ....

---

# Install vcpkg
```bash
git clone https://github.com/Microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
vcpkg install mpi
vcpkg integrate install
```

# Install WSL with ubuntu
```bash
sudo apt-get update
sudo apt-get upgrade
sudo apt install mpich
```

# Run the code in WSL
```bash
mpiexec -n <number_of_processes> mpi_<name_of_implementation> <path_to_points.txt> <flags>
```
Depending on the implementation there are different `flags` available, generally they are:
- `-v` : verbose enabled during mergesort part of the algorithm 
- `-e` : enumerate the pairs of point with smallest distance
- `-p` : print the pairs of point with smallest distance
