#pragma once

#include <vector>
#include <cstddef>

typedef size_t coord_t;
typedef std::vector<std::vector<bool> > field_t;
typedef std::vector<bool> row_t;

class ILifeGame {
public:
  virtual void run(int numberOfSteps) = 0;
  virtual void runAndWait(int numberOfSteps) = 0;
  virtual size_t getWidth() = 0;
  virtual size_t getHeight() = 0;
  virtual bool unitAliveAt(coord_t x, coord_t y) = 0;
  virtual void stop() = 0;
  virtual int getCurrentStep() = 0;
  virtual ~ILifeGame() { }
};
