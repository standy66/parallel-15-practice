cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Parallel)
include_directories(lib/include life-pthread/include)
file(GLOB LIB_SOURCE lib/src/*.cpp)
file(GLOB LIFE_PTHREAD_SOURCE life-pthread/src/*.cpp)

add_library(lib ${LIB_SOURCE})
target_link_libraries(lib PRIVATE -pthread)

add_executable(life_pthread ${LIFE_PTHREAD_SOURCE})
target_link_libraries(life_pthread lib)
