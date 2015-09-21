#pragma once

#include <pthread.h>
#include <semaphore.h>

class Thread {
public:
  void join();
  bool isRunning();
  virtual ~Thread();

protected:
  Thread();
  virtual void routine() = 0;

private:
  Thread(Thread&) { }
  Thread(const Thread&) { }

  static void* routineWrapper(void*);
  pthread_t pthread;
};

class Mutex {
public:
  Mutex();
  virtual ~Mutex();
  void lock();
  void unlock();
  //true if the access granted
  bool trylock();

private:
  pthread_mutexattr_t mutexAttr;
  pthread_mutex_t mutex;
};

class Semaphore {
public:
  Semaphore(unsigned int initValue);
  virtual ~Semaphore();
  void wait();
  void post();
  //true if the value is decreased
  bool trywait();



private:
  sem_t semaphore;
}
