#include <vector>

#include "threading.hpp"

typedef std::vector<std::vector<bool> > field_t;

class MasterThread;

class LifeGame {
public:
  LifeGame(int width, int height, int threadCount);
  LifeGame(const field_t& field);
  virtual ~LifeGame();

  void run(int steps);
  int getWidth();
  int getHeight();
  bool unitAliveAt(int x, int y);
  void stop();
  int getCurrentStep();

private:
  field_t field;
  bool syncNeeded;
  int width;
  MasterThread *master;
  int height;

  void sync();
};


class WorkerThread: Thread {
  friend class MasterThread;
public:
  WorkerThread(int id, const field_t& field, int from, int to, MasterThread& master);

protected:
  virtual void routine() override;

  field_t field;
  int id;
  std::vector<bool> topLine;
  std::vector<bool> bottomLine;
  Semaphore topSem;
  Semaphore bottomSem;
  void step();
  MasterThread& master;
  WorkerThread* top;
  WorkerThread* bottom;
  virtual void syncBorders();
  int stepCount;

  void sendBorders();
  void receiveBorders();
};

class MasterThread: WorkerThread {
  friend class WorkerThread;
public:
  MasterThread(const field_t& field, int threadCount);
  ~MasterThread();
  void run(int steps);
  void stop();
  int getCurrentStep();
  field_t getField();


private:
  WorkerThread** workerThreads;
  int threadCount;
  Cond cond;
  Mutex mutex;
  int maxStepCount;
  virtual void syncBorders() override;

};
