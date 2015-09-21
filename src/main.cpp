#include <stdio.h>
#include <vector>
#include <iostream>
#include <map>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

#include "shell.hpp"
#include "life_game.hpp"

LifeGame* game = NULL;

int start(std::istream& args, std::ostream& out, std::ostream& err) {
  int width, height;
  args >> width >> height;
  if (game != NULL)
    delete game;
  game = new LifeGame(width, height);
  return 0;
}

int status(std::istream& args, std::ostream& out, std::ostream& err) {
  if (game == NULL) {
    err << "No game started";
  } else {
    int width = game->getWidth();
    int height = game->getHeight();
    for (int i = 0; i < width; ++i) {
      for (int j = 0; j < height; ++j) {
        if (game->unitAliveAt(i, j))
          out << "*";
        else
          out << " ";
      }
      out << std::endl;
    }
  }
  return 0;
}

int run(std::istream& args, std::ostream& out, std::ostream& err) {
  if (game == NULL) {
    err << "No game started";
  } else {
    int k;
    args >> k;
    for (int i = 0; i < k; ++i) {
      game->nextStep();
    }
  }
  return 0;
}

int animate(std::istream& args, std::ostream& out, std::ostream& err) {
  if (game == NULL) {
    err << "No game started";
  } else {
    int delay;
    args >> delay;
    int width = game->getWidth();
    int height = game->getHeight();
    while (true) {
      system("clear");
      for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
          if (game->unitAliveAt(i, j))
            out << "*";
          else
            out << " ";
        }
        out << std::endl;
      }
      usleep(delay * 1000);
      game->nextStep();
    }
  }
  return 0;
}

int main() {
  std::map<std::string, Shell::Action> actionMap;
  actionMap["start"] = start;
  actionMap["run"] = run;
  actionMap["status"] = status;
  actionMap["animate"] = animate;
  Shell shell(actionMap);
  shell.run(std::cin, std::cout, std::cerr);
  return 0;
}
