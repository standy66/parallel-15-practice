#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>

#include "dbg.hpp"
#include "exceptions.hpp"
#include "utils.hpp"
#include "shell.hpp"
#include "life_game_interface.hpp"
#include "life_game_singlethreaded.hpp"

using utility::intVal;
using utility::toString;

Mutex m;

void inline argsCheck(const Shell::Args& args, size_t from, size_t to,
  std::ostream& out, const std::string& usage) {
  if (args.size() < from || args.size() > to) {
    out << "Usage: " << usage << std::endl;
    throw ActionException("Invalid number of arguments. Expected from " +
    toString(from) + " to " + toString(to));
  }
}

void nodes(const Shell::Args& args, std::ostream& out) {
  argsCheck(args, 1, 1, out, "nodes");
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  out << size << " nodes connected" << std::endl;
}

void start(const Shell::Args& args, std::ostream& out) {
  argsCheck(args, 2, 3, out, "start <width> <height> or start <csvFile>");
  field_t field;
  if (args.size() == 3) {
    int width = intVal(args[1]);
    int height = intVal(args[2]);
    if (width <= 0 || height <= 0) {
      throw ActionException("width, height and threadsCount must be positive");
    }
    ILifeGame* game = LifeGameSingleThreaded::random(width, height);
    field = game->getField();
  } else {
    std::string filename = args[1];
    std::fstream csvFile(filename.c_str());
    std::string line;
    std::string token;
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
  }
  DBG(toString(field));

  //TODO: send field
}
void quit(const Shell::Args& args, std::ostream& out) {
  argsCheck(args, 1, 1, out, "quit");
  throw ShellSilentInterruptException("quit");
}

void master() {
    std::map<std::string, Shell::Action> actionMap;
    actionMap["nodes"] = nodes;
    actionMap["start"] = start;
    actionMap["quit"] = quit;
    Shell shell(actionMap);
    shell.run(std::cin, std::cout, std::cout);
}

void slave() {

}

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    master();
  } else {
    slave();
  }
  MPI_Finalize();
  return 0;
}
