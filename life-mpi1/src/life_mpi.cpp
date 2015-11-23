#include <life_mpi.h>

#include <cstdlib>
#include <mpi.h>

#define FIELD_SEND_SIZES_TAG 0
#define FIELD_SEND_TAG 1

namespace {
  size_t getEnoughBufferSize(size_t width, size_t height) {
    return 2 * sizeof(size_t) + width * height * sizeof(size_t);
  }

  void pack(const field_t& field, size_t* buf, size_t from, size_t to, size_t& len) {
    size_t width = to - from;
    size_t height = field[0].size();
    buf[0] = width;
    buf[1] = height;
    for (size_t i = from; i < to; ++i) {
      for (size_t j = 0; j < height; ++j) {
        buf[2 + i * height + j] = field[i][j];
      }
    }
    len = 2 + width * height;
  }

  field_t unpack(size_t* mem) {
    size_t width = mem[0];
    size_t height = mem[1];
    field_t field(width, row_t(height, 0));
    for (size_t i = 0; i < width; ++i) {
      for (size_t j = 0; j < height; ++j) {
        field[i][j] = mem[2 + i * height + j];
      }
    }
    return field;
  }
}

LifeGameMpi* LifeGameMpiMaster::fromField(const field_t& field) {
  LifeGameMpi* obj = new LifeGameMpi();
  MPI_Comm_size(MPI_COMM_WORLD, &obj->commSize);
  obj->field = field;
  obj->width = field.size();
  obj->height = field[0].size();
  return obj;
}

LifeGameMpi* LifeGameMpiMaster::random(size_t width, size_t height) {
  LifeGameMpi* obj = new LifeGameMpi(width, height);
  MPI_Comm_size(MPI_COMM_WORLD, &obj->commSize);
  for (size_t i = 0; i < width; ++i) {
    for (size_t j = 0; j < height; ++j) {
      obj->field[i][j] = rand() % 2;
    }
  }
  return obj;
}

void LifeGameMpiMaster::run(size_t stepCount) {
  sendField();
  //for (size_t i = 0; i < stepCount; ++i) {

  //}
}

void LifeGameMpiMaster::sendField() {
  size_t widthPerNode = width / commSize + 1;
  buf = (size_t*)malloc(::getEnoughBufferSize(widthPerNode, height));
  size_t[] sizes = {widthPerNode, height};
  int curPos = 0;
  for (size_t i = 1; i < commSize; ++i) {
      MPI_Send(sizes, 2, MPI_UNSIGNED_LONG, i, FIELD_SEND_SIZES_TAG, MPI_COMM_WORLD);
      size_t sz;
      ::pack(field, buf, curPos, max(width, curPos + widthPerNode), sz);
      MPI_Send(buf, sz, MPI_UNSIGNED_LONG, i, FIELD_SEND_TAG, MPI_COMM_WORLD);
  }
}

//============= Life Game Mpi Slave =========================

LifeGameMpiSlave::LifeGameMpiSlave() {
  MPI_Comm_size(MPI_COMM_WORLD, &commSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &commRank);
}

void LifeGameMpiSlave::run() {
  recieveField();
}

void LifeGameMpiSlave::syncBorders() {

}

void LifeGameMpiSlave::recieveField() {
  size_t sizes[2];
  MPI_Status status;
  MPI_Recv(sizes, 2, MPI_UNSIGNED_LONG, 0, FIELD_SEND_SIZES_TAG, MPI_COMM_WORLD, &status);
  buf = (size_t*)malloc(::getEnoughBufferSize(width, height));
  MPI_Recv(buf, sizeof(buf) / sizeof(size_t), MPI_UNSIGNED_LONG, 0, FIELD_SEND_TAG, MPI_COMM_WORLD, &status);
}
