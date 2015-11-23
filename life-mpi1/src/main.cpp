#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <vector>

using namespace std;

void master() {

}

void slave() {

}

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  printf("rank: %d, size: %d\n", rank, size);
  MPI_Finalize();
  return 0;
}
