#pragma once

#include <vector>
#include <cstddef>

#include <life_game_interface.hpp>

class LifeGameMpiMaster {
public:
  LifeGameMpiMaster(const field_t &field);
  void run(size_t step1Count);
  void stop();

  virtual ~LifeGameMpiMaster();
private:
  void sendField();
  field_t field;
  size_t width;
  size_t height;
  size_t *buf;
};

class LifeGameMpiSlave {
public:
  LifeGameMpiSlave();

  void run();
private:
  field_t field;
  size_t width;
  size_t height;
  size_t *buf;

  void syncBorders();
  void recieveField();

};
