#include "life_game_openmp.hpp"

#include <cstdlib>

#include "exceptions.hpp"

void LifeGameOpenMP::run(int numberOfSteps) {
  for (int step = 0; step < numberOfSteps; ++step) {
    field_t cpy = field;
    #pragma omp parallel for
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

LifeGameOpenMP::~LifeGameOpenMP() { }

ILifeGame* LifeGameOpenMP::fromField(const field_t& field) {
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
  LifeGameOpenMP* game = new LifeGameOpenMP();
  game->field = field;
  game->width = width;
  game->height = height;
  return game;
}

ILifeGame* LifeGameOpenMP::random(size_t width, size_t height) {
  LifeGameOpenMP* game = new LifeGameOpenMP();
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
