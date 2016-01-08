#include <mpi.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <stdlib.h>

using namespace std;

#include "dbg.hpp"
#include "exceptions.hpp"
#include "utils.hpp"
#include "life_game_interface.hpp"
#include "life_game_singlethreaded.hpp"

using namespace utility;

Mutex m;

void inline argsCheck(int argc, size_t from, size_t to,
std::ostream& out, const std::string& usage) {
  if (argc < from || argc > to) {
    out << "Usage: " << usage << std::endl;
    throw ActionException("Invalid number of arguments. Expected from " +
    toString(from) + " to " + toString(to));
  }
}

inline double rtClock() {
  timespec tm;
  clock_gettime(CLOCK_REALTIME, &tm);
  double res = tm.tv_sec;
  res += tm.tv_nsec * 1e-9;
  return res;
}

bool *field, *cpy;
int width;
int height;
int iters;
int rank;
int size;

field_t load_field(int argc, char **argv) {
  argsCheck(argc, 3, 4, cout, "start <width> <height> <steps> or start <csvFile> <steps>");
  field_t field;
  if (argc == 4) {
    int width = intVal(string(argv[1]));
    int height = intVal(string(argv[2]));
    iters = intVal(string(argv[3]));
    if (width <= 0 || height <= 0) {
      throw ActionException("width, height and threadsCount must be positive");
    }
    ILifeGame* game = LifeGameSingleThreaded::random(width, height);
    field = game->getField();
  } else {
    std::string filename = string(argv[1]);
    iters = intVal(string(argv[2]));
    std::fstream csvFile(filename.c_str());
    std::string line;
    std::string token;
    while (std::getline(csvFile, line)) {
      std::vector<bool> row;
      std::istringstream lineStream(line);
      while (std::getline(lineStream, token, ',')) {
        if (token != ",") {
          int value = intVal(token);
          row.push_back(value);
        }
      }
      field.push_back(row);
    }
    if (field.size() == 0) {
      throw ActionException("File " + filename + " not found or empty.");
    }
  }
  DBG(toString(field));
  return field;
}

void iteration() {
  int from = width / size * rank;
  int to = from + width / size;
  for (int i = from; i < to; ++i) {
    for (int j = 0; j < height; ++j) {
      int numAlive = 0;
      for (int di = -1; di <= 1; ++di) {
        for (int dj = -1; dj <= 1; ++dj) {
          int ni = i + di;
          int nj = j + dj;

          if (ni == -1)
            ni = width - 1;
          else if (ni == width)
            ni = 0;

          if (nj == -1)
            nj = height - 1;
          else if (nj >= height)
            nj = 0;

          numAlive += field[ni * height + nj];
        }
      }
      numAlive -= field[i * height + j];
      if (numAlive == 3 || (numAlive == 2 && field[i * height + j]))
        cpy[i * height + j] = true;
      else
        cpy[i * height + j] = false;
    }
  }
  memcpy(field, cpy, width * height * sizeof(bool));
}

void sync_field() {
  std::cout << "all gather" << std::endl;
  MPI_Allgather(field, width / size * height, MPI::BOOL, field, width / size * height, MPI::BOOL, MPI_COMM_WORLD);
}

void compute() {
  for (int iter = 0; iter < iters; ++iter) {
    std::cout << "iter " << iter;
    iteration();
    sync_field();
  }
}

void master(int sz, int rk, int argc, char **argv) {
  size = sz;
  rank = rk;
  field_t fieldv = load_field(argc, argv);
  width = fieldv.size();
  height = fieldv[0].size();
  int dimens[] = {width, height, iters};
  MPI_Bcast(dimens, 3, MPI_INT, 0, MPI_COMM_WORLD);
  DBG(rank << ": " << width << " " << height << " " << iters);
  field = (bool*)malloc(sizeof(bool) * width * height);
  cpy = (bool*)malloc(sizeof(bool) * width * height);
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      field[i * height + j] = fieldv[i][j];
    }
  }
  MPI_Bcast(field, width * height, MPI::BOOL, 0, MPI_COMM_WORLD);
  double t = rtClock();
  compute();
  double delta = rtClock() - t;

  std::cout << "time " << delta << std::endl;
  std::cout << "ans done";

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      //std::cout << field[i * height + j];
    }
    //std::cout << endl;
  }

  free(field);
  free(cpy);
}

void slave(int sz, int rk) {
  size = sz;
  rank = rk;
  int dimens[3];
  MPI_Bcast(dimens, 3, MPI_INT, 0, MPI_COMM_WORLD);
  width = dimens[0];
  height = dimens[1];
  iters = dimens[2];
  DBG(rank << ": " << width << " " << height << " " << iters);
  field = (bool*)malloc(sizeof(bool) * width * height);
  cpy = (bool*)malloc(sizeof(bool) * width * height);
  MPI_Bcast(field, width * height, MPI::BOOL, 0, MPI_COMM_WORLD);
  compute();

  free(field);
  free(cpy);
}

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    master(size, rank, argc, argv);
  } else {
    slave(size, rank);
  }
  MPI_Finalize();
}
