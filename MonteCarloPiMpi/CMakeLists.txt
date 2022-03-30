cmake_minimum_required(VERSION 3.21)
project(MonteCarloPiMpi)

set(CMAKE_CXX_STANDARD 14)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
SET(CMAKE_CXX_COMPILER mpicxx)

add_executable(MonteCarloPiMpi main.c)
