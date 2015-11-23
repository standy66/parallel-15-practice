#include "life_game_singlethreaded.hpp"

#include <cstdlib>

#include "exceptions.hpp"

field_t LifeGameSingleThreaded::getField() {
  return field;
}

void LifeGameSingleThreaded::run(int numberOfSteps) {
  for (int i = 0; i < numberOfSteps; ++i) {
    field_t cpy = field;
    for (int i = 0; i < (int)width; ++i) {
      for (int j = 0; j < (int)height; ++j) {
        int numAlive = 0;
        for (int di = -1; di <= 1; ++di) {
          for (int dj = -1; dj <= 1; ++dj) {
            int ni = i + di;
            int nj = j + dj;

            if (ni == -1)
              ni = width - 1;
            else if ((size_t)ni == width)
              ni = 0;

            if (nj == -1)
              nj = height - 1;
            else if ((size_t)nj >= height)
              nj = 0;

            numAlive += field[ni][nj];
          }
        }
        numAlive -= field[i][j];
        if (numAlive == 3 || (numAlive == 2 && field[i][j]))
          cpy[i][j] = true;
        else
          cpy[i][j] = false;
      }
    }
    field = cpy;
  }
}

void LifeGameSingleThreaded::runAndWait(int numberOfSteps) {
  run(numberOfSteps);
}

size_t LifeGameSingleThreaded::getWidth() {
  if (width > 50)
    return 50;
  return width;
}

size_t LifeGameSingleThreaded::getHeight() {
  if (height > 50)
    return 50;
  return height;
}

bool LifeGameSingleThreaded::unitAliveAt(coord_t x, coord_t y) {
  if (x >= width || y >= height) {
    throw IllegalArgumentException("invalid coordinates");
  }
  return field[x][y];
}

void LifeGameSingleThreaded::stop() {
  //do nothing
}

int LifeGameSingleThreaded::getCurrentStep() {
  return currentStep;
}

LifeGameSingleThreaded::~LifeGameSingleThreaded() { }

ILifeGame* LifeGameSingleThreaded::fromField(const field_t& field) {
  size_t width = field.size();
  size_t height = 0;
  if (width > 0) {
    height = field[0].size();
    for (size_t i = 0; i < width; ++i) {
      if (height != field[i].size()) {
        throw IllegalArgumentException("field not rectangular");
      }
    }
  }
  LifeGameSingleThreaded* game = new LifeGameSingleThreaded();
  game->field = field;
  game->width = width;
  game->height = height;
  return game;
}

ILifeGame* LifeGameSingleThreaded::random(size_t width, size_t height) {
  LifeGameSingleThreaded* game = new LifeGameSingleThreaded();
  game->width = width;
  game->height = height;

  for (size_t i = 0; i < width; ++i) {
    game->field.push_back(row_t());
    for (size_t j = 0; j < height; ++j) {
      game->field[i].push_back(rand() % 2);
    }
  }
  return game;
}
