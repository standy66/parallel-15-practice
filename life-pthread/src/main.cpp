#include <stdio.h>
#include <vector>
#include <iostream>
#include <map>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <ctime>

#include "shell.hpp"
#include "life_game.hpp"
#include "life_game_singlethreaded.hpp"
#include "utils.hpp"
#include "exceptions.hpp"
#include "threading.hpp"
#include "dbg.hpp"

ILifeGame* game = NULL;

Mutex m;

using namespace utility;

void inline argsCheck(const Shell::Args& args, size_t from, size_t to,
  std::ostream& out, const std::string& usage) {
  if (args.size() < from || args.size() > to) {
    out << "Usage: " << usage << std::endl;
    throw ActionException("Invalid number of arguments. Expected from " +
    toString(from) + " to " + toString(to));
  }
}

void start(const Shell::Args& args, std::ostream& out) {
  argsCheck(args, 3, 4, out, "start <width> <height> <threadCount> or start <csvFile> <threadCount>");
  if (args.size() == 4) {
    int width = intVal(args[1]);
    int height = intVal(args[2]);
    int threads = intVal(args[3]);
    if (width <= 0 || height <= 0 || threads <= 0) {
      throw ActionException("width, height and threadsCount must be positive");
    }
    if (game != NULL)
      delete game;
    if (threads > 1) {
      game = LifeGame::random(width, height, threads);
    } else {
      game = LifeGameSingleThreaded::random(width, height);
    }
  } else {
    std::string filename = args[1];
    int threads = intVal(args[2]);
    if (threads <= 0) {
      throw ActionException("threadsCount must be positive");
    }
    std::fstream csvFile(filename.c_str());
    std::string line;
    std::string token;
    field_t field;
    while (std::getline(csvFile, line)) {
      std::vector<bool> row;
      std::istringstream lineStream(line);
      while (std::getline(lineStream, token, ',')) {
        DBG(token);
        if (token != ",") {
          int value = intVal(token);
          row.push_back(value);
        }
      }
      field.push_back(row);
    }
    DBG(toString(field));
    if (field.size() == 0) {
      throw ActionException("File " + filename + " not found or empty.");
    }
    if (threads > 1) {
      game = LifeGame::fromField(field, threads);
    } else {
      game = LifeGameSingleThreaded::fromField(field);
    }
  }
}

void status(const Shell::Args& args, std::ostream& out) {
  argsCheck(args, 1, 1, out, "status");
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
  argsCheck(args, 2, 2, out, "run <stepCount>");
  if (game == NULL) {
    throw ActionException("No game started");
  } else {
    int k = intVal(args[1]);
    game->run(k);
  }
}

void runwait(const Shell::Args& args, std::ostream& out) {
  argsCheck(args, 2, 2, out, "runwait <stepCount>");
  if (game == NULL) {
    throw ActionException("No game started");
  } else {
    int k = intVal(args[1]);
    double time = rtClock();
    game->runAndWait(k);
    double delta = rtClock() - time;
    out << "Done. Elapsed time: " << delta << " s" << std::endl;
  }
}

void quit(const Shell::Args& args, std::ostream& out) {
  argsCheck(args, 1, 1, out, "quit");
  delete game;
  throw ShellSilentInterruptException("quit");
}

void stop(const Shell::Args& args, std::ostream& out) {
  argsCheck(args, 1, 1, out, "stop");
  if (game == NULL) {
    throw ActionException("No game started");
  }
  game->stop();
  out << game->getCurrentStep();
}
/*void animate(const Shell::Args& args, std::ostream& out) {
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
}*/


int main() {
  std::map<std::string, Shell::Action> actionMap;
  actionMap["start"] = start;
  actionMap["run"] = run;
  actionMap["status"] = status;
  actionMap["stop"] = stop;
  actionMap["quit"] = quit;
  actionMap["runwait"] = runwait;
  //actionMap["animate"] = animate;
  Shell shell(actionMap);
  shell.run(std::cin, std::cout, std::cerr);
  return 0;
}
