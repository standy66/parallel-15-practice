#include "threading.hpp"

#include <errno.h>
#include <string>
#include <string.h>

#include "exceptions.hpp"

//===== Thread =====

void Thread::join() {
  //TODO: sometimes wait can be interrupted by a signal handler
  // so we should check for it and don't throw any exceptions
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
  //TODO: sometimes wait can be interrupted by a signal handler
  // so we should check for it and don't throw any exceptions
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

//===== Semaphore =====

Semaphore::Semaphore(unsigned int initValue) {
  if (sem_init(&semaphore, 0, initValue) != 0) {
    THROW_LEGACY_EXCEPTION
  }
}

Semaphore::~Semaphore() {
  if (sem_destroy(&semaphore) != 0) {
    THROW_LEGACY_EXCEPTION
  }
}

void Semaphore::wait() {
  //TODO: sometimes wait can be interrupted by a signal handler
  // so we should check for it and don't throw any exceptions
  if (sem_wait(&semaphore) != 0) {
    THROW_LEGACY_EXCEPTION
  }
}

void Semaphore::post() {
  if (sem_post(&semaphore) != 0) {
    THROW_LEGACY_EXCEPTION
  }
}

bool Semaphore::trywait() {
  if (sem_trywait(&semaphore) == 0) {
    return true;
  } else {
    if (errno = EAGAIN) {
      return false;
    } else {
      THROW_LEGACY_EXCEPTION
    }
  }
}

//===== end Semaphore =====
