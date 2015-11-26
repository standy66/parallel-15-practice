#pragma once

#include <vector>
#include <cstddef>
#include <queue>
#include <utility>
#include <mpi.h>

#include <life_game_interface.hpp>

class LifeGameMpiMaster {
public:
  LifeGameMpiMaster(const field_t &field);
  void run(size_t stepCount);
  void runwait(size_t stepCount);
  void stop();
  void sync();


  virtual ~LifeGameMpiMaster() { }

  bool running;
  field_t field;
  size_t width;
  size_t height;
  int maxVersion;
private:
  int nodesCount;
  char* buf;
  size_t bufSize;
};

class LifeGameMpiSlave {
public:
  LifeGameMpiSlave();

  void run();
private:
  bool running;
  bool detached;
  std::queue<row_t> leftBorderQueue;
  std::queue<row_t> rightBorderQueue;
  size_t version;
  size_t maxVersion;
  field_t field;
  size_t width;
  size_t height;
  int nodesCount;
  bool handleMessage();
  void adjustBuffer(MPI_Status& status, MPI_Datatype);
  void adjustBuffer(size_t reqSize);
  char* buf;
  size_t bufSize;
};
