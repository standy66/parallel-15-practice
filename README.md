# parallel-15-practice
Repository for 2015 Parallel and Distributed Computing course at MIPT

The first task was to implement Conway's game of life in a multithreaded approach using pthreads. I have used pthread_mutex_t, pthread_cond_t, sem_t (see threading.cpp) to attain synchronization between the threads. For a better grasp and comprehension of the approach see code in life_game.cpp. Lots of auxiluary files procure better abstraction and architecture design, though regrettably I am not as good in C++ code architecture design as Fedor Alexeev, but I do my best :)

And BTW, I've pulled off 1.6 speedup rate on 2-core system and 3.1 on 4-core (without HT or the stuff alike)
