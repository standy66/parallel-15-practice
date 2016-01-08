#include "threading.hpp"

#include <errno.h>
#include <string>
#include <string.h>
#include <typeinfo>

#include "exceptions.hpp"
#include "dbg.hpp"
#include "utils.hpp"

//===== Thread =====

void Thread::join() {
  if (!running)
    return;
  //TODO: sometimes wait can be interrupted by a signal handler
  // so we should check for it and don't throw any exceptions
  int res = pthread_join(pthread, NULL);
  if (res != 0) {
    DBG(res << ESRCH);
    THROW_LEGACY_EXCEPTION
  }
  running = false;
}

bool Thread::isRunning() {
  return running;
}

Thread::Thread() :running(false) { }

Thread::~Thread() { }

void Thread::run() {
  if (!running) {
    running = true;
    if (pthread_create(&pthread, NULL, routineWrapper, this) != 0) {
      running = false;
      THROW_LEGACY_EXCEPTION
    }
    //int oldType = 0;
    //pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldType);
  }
}

void* Thread::routineWrapper(void* arg) {
  Thread* thread = static_cast<Thread*>(arg);
  thread->routine();
  return NULL;
}


void Thread::cancel() {
  pthread_cancel(pthread);
}

//===== end Thread =====


//===== Mutex =====

Mutex::Mutex() {
  if (pthread_mutexattr_init(&mutexAttr) != 0) {
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

int Semaphore::sem_count = 0;

Semaphore::Semaphore(unsigned int initValue) {
  #ifdef __MACH__
  std::string prefix = "mipt_threading";
  std::string name = prefix + utility::toString(sem_count);
  psem = sem_open(name.c_str(), O_CREAT, 777, initValue);

  #else
  if (sem_init(&semaphore, 0, initValue) != 0) {
    THROW_LEGACY_EXCEPTION
  }
  #endif
}

Semaphore::~Semaphore() {
  #ifdef __MACH__
  sem_close(psem);
  #else
  if (sem_destroy(&semaphore) != 0) {
    THROW_LEGACY_EXCEPTION
  }
  #endif
}

void Semaphore::wait() {
  //TODO: sometimes wait can be interrupted by a signal handler
  // so we should check for it and don't throw any exceptions
  #ifdef __MACH__
  if (sem_wait(psem) != 0) {
    THROW_LEGACY_EXCEPTION
  }
  #else
  if (sem_wait(&semaphore) != 0) {
    THROW_LEGACY_EXCEPTION
  }
  #endif
}

void Semaphore::post() {
  #ifdef __MACH__
  if (sem_post(psem) != 0) {
    THROW_LEGACY_EXCEPTION
  }
  #else
  if (sem_post(&semaphore) != 0) {
    THROW_LEGACY_EXCEPTION
  }
  #endif
}

bool Semaphore::trywait() {
  #ifdef __MACH__
  if (sem_trywait(psem) == 0) {
    return true;
  } else {
    if (errno == EAGAIN) {
      return false;
    } else {
      THROW_LEGACY_EXCEPTION
    }
  }
  #else
  if (sem_trywait(&semaphore) == 0) {
    return true;
  } else {
    if (errno == EAGAIN) {
      return false;
    } else {
      THROW_LEGACY_EXCEPTION
    }
  }
  #endif
}

//===== end Semaphore =====

//===== Cond =====

Cond::Cond() {
  if (pthread_condattr_init(&condAttr) != 0) {
    THROW_LEGACY_EXCEPTION
  }
  if (pthread_cond_init(&cond, &condAttr) != 0) {
    THROW_LEGACY_EXCEPTION
  }
}

Cond::~Cond() {
  if (pthread_condattr_destroy(&condAttr) != 0) {
    THROW_LEGACY_EXCEPTION
  }
  notifyAll();
  if (pthread_cond_destroy(&cond) != 0) {
    THROW_LEGACY_EXCEPTION
  }
}

void Cond::notify() {
  if (pthread_cond_signal(&cond) != 0) {
    THROW_LEGACY_EXCEPTION
  }
}

void Cond::notifyAll() {
  if (pthread_cond_broadcast(&cond) != 0) {
    THROW_LEGACY_EXCEPTION
  }
}

void Cond::wait(Mutex& m) {
  if (pthread_cond_wait(&cond, &m.mutex) != 0) {
    THROW_LEGACY_EXCEPTION
  }
}


//===== end Cond =====
