#include "life_game.hpp"
#include "dbg.hpp"
#include "utils.hpp"
#include "exceptions.hpp"

#include <cstdlib>

using utility::toString;

LifeGame::LifeGame(size_t width, size_t height, int threadCount)
: field(width, std::vector<bool>(height, false)), syncNeeded(false) {
  for (size_t i = 0; i < width; ++i) {
    for (size_t j = 0; j < height; ++j) {
      field[i][j] = rand() % 2;
    }
  }
  this->width = width;
  this->height = height;
  master = new MasterThread(field, threadCount);
}

LifeGame::LifeGame(const field_t& field, int threadCount)
: field(field), syncNeeded(false) {
  this->width = field.size();
  this->height = field[0].size();
  master = new MasterThread(field, threadCount);
}

ILifeGame* LifeGame::fromField(const field_t& field, int threadCount) {
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
  return new LifeGame(field, threadCount);
}

ILifeGame* LifeGame::random(size_t width, size_t height, int threadCount) {
  return new LifeGame(width, height, threadCount);
}

LifeGame::~LifeGame() {
  delete master;
}

void LifeGame::run(int steps) {
  master->run(steps);
  syncNeeded = true;
}

void LifeGame::runAndWait(int steps) {
  master->run(steps);
  master->wait();
  syncNeeded = true;
}

size_t LifeGame::getWidth() {
  sync();
  if (width > 50)
    return 50;
  return width;
}

size_t LifeGame::getHeight() {
  sync();
  if (height > 50)
    return 50;
  return height;
}

bool LifeGame::unitAliveAt(coord_t x, coord_t y) {
  sync();
  if (x >= width || y >= height) {
    throw IllegalArgumentException("invalid coordinates");
  }
  return field[x][y];
}

void LifeGame::stop() {
  master->stop();
}

int LifeGame::getCurrentStep() {
  return master->getCurrentStep();
}

void LifeGame::sync() {
  if (syncNeeded) {
    syncNeeded = false;
    field = master->getField();
    width = field.size();
    height = field[0].size();
  }
}

MasterThread::MasterThread(const field_t& field, int threadCount)
:WorkerThread(-1, field, 0, 0, *this), threadCount(threadCount), maxStepCount(0) {
  workerThreads = new WorkerThread*[threadCount];
  int chunkSize = field.size() / threadCount;
  int currentChunk = 0;
  for (int i = 0; i < threadCount; ++i) {
    workerThreads[i] = new WorkerThread(i, field, currentChunk,
      (i == threadCount - 1) ? field.size() : currentChunk + chunkSize,
      *this);
    currentChunk += chunkSize;
  }
  for (int i = 0; i < threadCount; ++i) {
    workerThreads[i]->top = workerThreads[(i - 1 + threadCount) % threadCount];
    workerThreads[i]->bottom = workerThreads[(i + 1 + threadCount) % threadCount];
  }
  top = workerThreads[threadCount - 1];
  bottom = workerThreads[0];
  bottom->top = this;
  top->bottom = this;
  running = true;
}

MasterThread::~MasterThread() {
  running = false;
  run(1000);
  join();
  for (int i = 0; i < threadCount; ++i) {
    workerThreads[i]->join();
    delete workerThreads[i];
  }
  delete[] workerThreads;
}

void MasterThread::syncBorders() {
  mutex.lock();
  while (stepCount >= maxStepCount) {
    joinMutex.lock();
    joinCond.notifyAll();
    joinMutex.unlock();

    cond.wait(mutex);
  }
  mutex.unlock();
  //WorkerThread::syncBorders();

  receiveBorders();
  sendBorders();
}

void MasterThread::wait() {
  joinMutex.lock();
  while (stepCount < maxStepCount) {
    joinCond.wait(joinMutex);
  }
  joinMutex.unlock();
}

void MasterThread::run(int steps) {
  Thread::run();
  for (int i = 0; i < threadCount; ++i) {
    workerThreads[i]->run();
  }
  mutex.lock();
  maxStepCount += steps;
  cond.notify();
  mutex.unlock();
}

void MasterThread::stop() {
  mutex.lock();
  maxStepCount = stepCount;
  mutex.unlock();
}

int MasterThread::getCurrentStep() {
  return stepCount;
}

field_t MasterThread::getField() {
  field_t field;
  for (int i = 0; i < threadCount; ++i) {
    for (size_t j = 1; j < workerThreads[i]->field.size() - 1; ++j) {
      field.push_back(workerThreads[i]->field[j]);
    }
  }
  return field;
}

WorkerThread::WorkerThread(int id, const field_t& allField, int from, int to,
  MasterThread& master)
:id(id), topSem(0), bottomSem(0),
 topRead(1), bottomRead(1), master(master),  stepCount(0) {
  //DBG("thread " << id << " got allField " << toString(allField));
  //DBG("thread " << id << " got from " << from << " to " << to);
  //DBG("thread " << id << " allField size " << allField.size());
  for (int i = from - 1; i < to + 1; ++i) {
    //DBG("i " << i);
    int normi = (i + allField.size()) % allField.size();
    //DBG("normi " << normi);
    //DBG("allField[normi] " << toString(allField[normi]));
    field.push_back(allField[normi]);
    //DBG("field " << toString(field));
  }
  //DBG("thread " << id << " initialized field " << toString(field));
}

void WorkerThread::routine() {
  //TODO: what if this is started before initialization
  while (master.running) {
    stepCount++;
    DBG("thread" << id << " step " << stepCount);
    //master.mutex.lock();
    //while (!running) {
    //  master.stopCond.wait(master.stopMutex);
    //}
    //master.mutex.unlock();
    step();
    DBG("thread " << id << " preparing to sync");
    syncBorders();
    DBG("thread " << id << "synced borders");
  }
}

void WorkerThread::step() {
  field_t cpy = field;
  for (size_t i = 1; i < field.size() - 1; ++i) {
    for (size_t j = 0; j < field[i].size(); ++j) {
      int height = field[i].size();
      int numAlive = 0;
      for (int di = -1; di <= 1; ++di) {
        for (int dj = -1; dj <= 1; ++dj) {
          int nj = j + dj;
          if (nj < 0)
            nj += height;
          if (nj >= height)
            nj -= height;
          numAlive += field[i + di][nj];
        }
      }
      numAlive -= field[i][j];
      if (numAlive == 3 || (numAlive == 2 && field[i][j]))
        cpy[i][j] = true;
      else
        cpy[i][j] = false;
    }
  }
  DBG("thread " << id << "field was " << std::endl << toString(field)
      << " now " << std::endl << toString(cpy));
  field = cpy;
}

void WorkerThread::syncBorders() {
  sendBorders();
  receiveBorders();
}

void WorkerThread::sendBorders() {
  if (top != NULL) {
    top->bottomRead.wait();
    top->bottomLine = field[1];
    DBG("thead " << id << " sent top->bottomLine " << toString(field[1]));
    top->bottomSem.post();
  }
  if (bottom != NULL) {
    bottom->topRead.wait();
    bottom->topLine = field[field.size() - 2];
    DBG("thread " << id << " sent bottom->topLine " << toString(field[field.size() - 2]));
    bottom->topSem.post();
  }
}

void WorkerThread::receiveBorders() {
  topSem.wait();
  DBG("thread " << id << " got topline " << toString(topLine));
  field[0] = topLine;
  topRead.post();
  bottomSem.wait();
  DBG("thread " << id << " got bottomline " << toString(bottomLine));
  field[field.size() - 1] = bottomLine;
  bottomRead.post();
}
