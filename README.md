## <p align="center">High Performance Computing for Data Science - Project</p> 

This repository is a student project developed by Kevin Depedri and Davide Modolo for the "High Performance Computing for Data Science" course of the Master in Artificial Intelligent Systems at the University of Trento, a.y. 2022-2023. 

This readme file presents all the steps to follow for ....

---

Distance between two points in $k$-dimensional space: $\displaystyle\text{distance}(p_a, p_b) = \sqrt{\sum_{i=0}^{k}(a_i - b_i)^2}$

TODO:

- [x] $n$ points in $k$ dimensions generator

- [x] sequential version of $O(n^2)$

- [ ] sequential versione of $O(n\log n)$ _Divide&Conquer_

- [ ] parallel version of $O(n^2)$

- [ ] parallel versione of $O(n\log n)$ _Divide&Conquer_

- [ ] if cores > points versions

- [ ] maybe double parallel version

# Install vcpkg
```bash
git clone https://github.com/Microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
vcpkg install mpi
vcpkg integrate install
```
