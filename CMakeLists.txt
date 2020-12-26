cmake_minimum_required(VERSION 3.16)
project(MySTL)

set(CMAKE_CXX_STANDARD 14)

add_executable(MySTL main.cpp MyAlgorithm.h JJAlloc.h iterator.h alloctor.h)