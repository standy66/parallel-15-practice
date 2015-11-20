#include <iostream>

#include "threading.hpp"

class CounterThread: public Thread {
public:
  static Mutex mutex;
  static int total;
  CounterThread(int threadId, int count)
    : threadId(threadId), count(count) { }

  virtual void routine() {
    for (int j = 0; j < 1000; j++) {
      //mutex.lock();
      total++;
      std::cout << total;
      /*std::cout << "[";
      for (int i = 0; i < count; ++i) {
        std::cout << threadId;
      }
      std::cout << "]";*/
      std::cout << total;
      total--;
      //mutex.unlock();
    }
  }

private:
  int count, threadId;

};

Mutex CounterThread::mutex;
int CounterThread::total = 0;

int main() {
    CounterThread t1(1, 200), t2(2, 200);
    std::cout << std::endl;
}
