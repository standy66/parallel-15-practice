#pragma once

#include <pthread.h>
#include <semaphore.h>

#include "utils.hpp"

using utility::NonCopyable;

class Thread: NonCopyable {
public:
  void join();
  bool isRunning();
  virtual ~Thread();
  void run();

protected:
  virtual void routine() = 0;
  Thread();

private:
  bool running;
  static void* routineWrapper(void*);
  pthread_t pthread;
};

class Mutex: NonCopyable {
  friend class Cond;
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

class Semaphore: NonCopyable {
public:
  Semaphore(unsigned int initValue);
  virtual ~Semaphore();
  void wait();
  void post();
  //true if the value is decreased
  bool trywait();

private:
  sem_t semaphore;
};

class Cond: NonCopyable {
public:
  Cond();
  virtual ~Cond();
  void notify();
  void notifyAll();
  void wait(Mutex& m);

private:
  pthread_cond_t cond;
  pthread_condattr_t condAttr;
};
