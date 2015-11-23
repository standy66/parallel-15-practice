cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(parallel)
find_package(MPI REQUIRED)
set(CMAKE_CXX_FLAGS "-Wall -pedantic -Wextra -std=c++03")
include_directories(${MPI_INCLUDE_PATH})
include_directories(lib/include life-pthread/include)
file(GLOB LIB_SOURCE lib/src/*.cpp)
file(GLOB LIFE_PTHREAD_SOURCE life-pthread/src/*.cpp)
file(GLOB LIGE_MPI_SOURCE life-mpi1/src/main.cpp)

add_library(lib ${LIB_SOURCE})
target_link_libraries(lib PRIVATE "-pthread")
add_executable(life_pthread ${LIFE_PTHREAD_SOURCE})
target_link_libraries(life_pthread lib)
add_executable(life_mpi ${LIGE_MPI_SOURCE})
target_link_libraries(life_mpi lib ${MPI_LIBRARIES})

if(MPI_COMPILE_FLAGS)
  set_target_properties(life_mpi PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(life_mpi PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()
