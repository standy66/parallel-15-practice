#include <vector>

#include "threading.hpp"
#include "life_game_interface.hpp"


//TODO: refactor this

class MasterThread;

class LifeGame: public ILifeGame {
public:
  virtual ~LifeGame();

  virtual void run(int steps);
  virtual size_t getWidth();
  virtual size_t getHeight();
  virtual bool unitAliveAt(coord_t x, coord_t y);
  virtual void stop();
  virtual int getCurrentStep();

  static ILifeGame* fromField(const field_t& field, int threadCount);
  static ILifeGame* random(size_t width, size_t height, int threadCount);

private:
  LifeGame(size_t width, size_t height, int threadCount);
  LifeGame(const field_t& field, int threadCount);

  field_t field;
  bool syncNeeded;
  size_t width;
  MasterThread *master;
  size_t height;

  void sync();
};


class WorkerThread: Thread {
  friend class MasterThread;
public:
  WorkerThread(int id, const field_t& field, int from, int to, MasterThread& master);

protected:
  virtual void routine();

  field_t field;
  int id;
  std::vector<bool> topLine;
  std::vector<bool> bottomLine;
  Semaphore topSem;
  Semaphore bottomSem;
  Semaphore topRead;
  Semaphore bottomRead;
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
  virtual void syncBorders();
  bool running;

};
