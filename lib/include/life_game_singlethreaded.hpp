#pragma once
#include "life_game_interface.hpp"

class LifeGameSingleThreaded: public ILifeGame {
public:
  virtual void run(int numberOfSteps);
  virtual void runAndWait(int numberOfSteps);
  virtual size_t getWidth();
  virtual size_t getHeight();
  virtual bool unitAliveAt(coord_t x, coord_t y);
  virtual void stop();
  virtual int getCurrentStep();
  virtual ~LifeGameSingleThreaded();
  virtual field_t getField();

  static ILifeGame* fromField(const field_t& field);
  static ILifeGame* random(size_t width, size_t height);

protected:
  field_t field;
  size_t width;
  size_t height;
  int currentStep;
};
