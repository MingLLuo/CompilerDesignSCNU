cmake_minimum_required(VERSION 3.26)
project(task2)

set(CMAKE_CXX_STANDARD 17)

add_executable(task2 main.cpp
  src/util.cpp
  src/util.h
  src/pattern.cpp
  src/pattern.h
  src/grammar.cpp
  src/grammar.h
  src/lr0Parser.cpp
  src/lr0Parser.h
  src/slr1Parser.cpp
  src/slr1Parser.h
)

target_include_directories(task2 PRIVATE src)
