cmake_minimum_required(VERSION 3.21)
project(OddEvenSortMpi C)

set(CMAKE_C_STANDARD 99)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
SET(CMAKE_CXX_COMPILER mpicxx)

add_executable(OddEvenSortMpi main.c)
