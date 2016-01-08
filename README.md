# parallel-15-practice
Repository for 2015 Parallel and Distributed Computing course at MIPT

Here are 3 versions of Conway's Game of Life:
- One that uses POSIX threads
- MPI
- MPI with collective operations (not yet fully working)
- OpenMP

Clang currently doesn't support OpenMP, so you should do

`export CC = <path to gcc on your system>`

`export CXX = <path to g++ on your system>`

before running ./makeall.sh
