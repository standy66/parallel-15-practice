#pragma once
#include "life_game_singlethreaded.hpp"

class LifeGameOpenMP: public LifeGameSingleThreaded {
public:
  virtual void run(int numberOfSteps);

  static ILifeGame* fromField(const field_t& field);
  static ILifeGame* random(size_t width, size_t height);
  virtual ~LifeGameOpenMP();
};
