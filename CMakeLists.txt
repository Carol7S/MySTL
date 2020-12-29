cmake_minimum_required(VERSION 3.16)
project(MySTL)

set(CMAKE_CXX_STANDARD 14)

add_executable(MySTL main.cpp JJAlloc.h iterator.h alloctor.h construct.h jjauto_ptr.h __type_traits.h uninitialized.h algobase.h algo.h algorithm.h vector.h list.h deque.h stack.h)