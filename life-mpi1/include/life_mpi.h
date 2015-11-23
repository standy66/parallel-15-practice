#pragma once

#include <vector>
#include <cstddef>

typedef size_t coord_t;
typedef std::vector<std::vector<bool> > field_t;
typedef std::vector<bool> row_t;

class LifeGameMpiMaster {
public:
  static LifeGameMpi* fromField(const field_t& field);
  static LifeGameMpi* random(size_t width, size_t height);

  void run(size_t step1Count);
  void stop();

  virtual ~LifeGameMpiMaster();
private:
  LifeGameMpi() { }
  LifeGameMpi(size_t width, size_t height)
    : width(width), height(height), field(width, row_t(0, height)) { }
  void sendField();
  field_t field;
  size_t width;
  size_t height;
  size_t commSize;
  size_t *buf;
};

class LifeGameMpiSlave {
public:
  LifeGameMpiSlave();

  void run();
private:1
  size_t commRank;
  size_t commSize;

  field_t field;
  size_t width;
  size_t height;
  size_t *buf;

  void syncBorders();
  void recieveField();

};
