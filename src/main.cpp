#include <stdio.h>
#include <vector>
#include <iostream>
#include <map>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

#include "shell.hpp"
#include "life_game.hpp"
#include "utils.hpp"
#include "exceptions.hpp"
#include "threading.hpp"

LifeGame* game = NULL;

using utility::intVal;
using utility::toString;

void inline argsCheck(const Shell::Args& args, int from, int to) {
  if (args.size() < from || args.size() > to) {
    throw ActionException("Invalid number of arguments.");
  }
}

void start(const Shell::Args& args, std::ostream& out) {
  argsCheck(args, 3, 3);
  int width = intVal(args[1]);
  int height = intVal(args[2]);
  if (game != NULL)
    delete game;
  game = new LifeGame(width, height);
}

void status(const Shell::Args& args, std::ostream& out) {
  argsCheck(args, 1, 1);
  if (game == NULL) {
    throw ActionException("No game started");
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
}

void run(const Shell::Args& args, std::ostream& out) {
  argsCheck(args, 2, 2);
  if (game == NULL) {
    throw ActionException("No game started");
  } else {
    int k = intVal(args[1]);
    for (int i = 0; i < k; ++i) {
      game->nextStep();
    }
  }
}

void animate(const Shell::Args& args, std::ostream& out) {
  argsCheck(args, 2, 2);
  if (game == NULL) {
    throw ActionException("No game started");
  } else {
    int delay = intVal(args[1]);
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
}

class CounterThread: public Thread {
public:
  static Mutex mutex;
  static int total;
  CounterThread(int threadId, int count)
    : threadId(threadId), count(count) { }

  virtual void routine() {
    for (int j = 0; j < 1000; j++) {
      //mutex.lock();
      total++;
      std::cout << total;
      /*std::cout << "[";
      for (int i = 0; i < count; ++i) {
        std::cout << threadId;
      }
      std::cout << "]";*/
      std::cout << total;
      total--;
      //mutex.unlock();
    }
  }

private:
  int count, threadId;

};

Mutex CounterThread::mutex;
int CounterThread::total = 0;

int main() {
  CounterThread t1(1, 200), t2(2, 200);
  std::cout << std::endl;
  std::map<std::string, Shell::Action> actionMap;
  actionMap["start"] = start;
  actionMap["run"] = run;
  actionMap["status"] = status;
  actionMap["animate"] = animate;
  Shell shell(actionMap);
  shell.run(std::cin, std::cout, std::cerr);
  return 0;
}
