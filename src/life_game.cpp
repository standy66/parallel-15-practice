#include "life_game.h"
#include "dbg.h"

#include <cstdlib>

LifeGame::LifeGame(int width, int height)
: field(width, std::vector<bool>(height, false)) {
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      field[i][j] = rand() % 2;
    }
  }
  this->width = width;
  this->height = height;
}

void LifeGame::nextStep() {
  field_t cpy = field;
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      int numAlive = 0;
      for (int di = -1; di <= 1; ++di) {
        for (int dj = -1; dj <= 1; ++dj) {
          numAlive += field[(i + di + width) % width][(j + dj + height) % height];
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

int LifeGame::getWidth() {
  return width;
}

int LifeGame::getHeight() {
  return height;
}

bool LifeGame::unitAliveAt(int x, int y) {
  if (x < 0 || x >= width || y < 0 || y >= height) {
    return false;
  }
  return field[x][y];
}
