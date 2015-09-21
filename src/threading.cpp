#include "threading.hpp"

#include <errno.h>
#include <string>
#include <string.h>

#include "exceptions.hpp"

//===== Thread =====

void Thread::join() {
  if (pthread_join(pthread, NULL) != 0) {
    THROW_LEGACY_EXCEPTION
  }
}

bool Thread::isRunning() {
  return true;
}

Thread::~Thread() {
  //TODO: should write something here
}

Thread::Thread() {
  if (pthread_create(&pthread, NULL, routineWrapper, this) != 0) {
    THROW_LEGACY_EXCEPTION
  }
}

void* Thread::routineWrapper(void* arg) {
  Thread* thread = static_cast<Thread*>(arg);
  thread->routine();
  return NULL;
}

//===== end Thread =====


//===== Mutex =====

Mutex::Mutex() {
  if (pthread_mutexattr_init(&mutexAttr) != 0) {
    THROW_LEGACY_EXCEPTION
  }
  if (pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_FAST_NP) != 0) {
    THROW_LEGACY_EXCEPTION
  }
  if (pthread_mutex_init(&mutex, &mutexAttr) != 0) {
    THROW_LEGACY_EXCEPTION
  }
}

Mutex::~Mutex() {
  pthread_mutexattr_destroy(&mutexAttr);
  pthread_mutex_destroy(&mutex);
}

void Mutex::lock() {
  if (pthread_mutex_lock(&mutex) != 0) {
    THROW_LEGACY_EXCEPTION
  }
}

void Mutex::unlock() {
  if (pthread_mutex_unlock(&mutex) != 0) {
    THROW_LEGACY_EXCEPTION
  }
}

bool Mutex::trylock() {
  int resultCode = pthread_mutex_trylock(&mutex);
  if (resultCode == 0) {
    return true;
  }
  if (resultCode == EBUSY) {
    return false;
  }
  THROW_LEGACY_EXCEPTION
}

//===== end Mutex =====
