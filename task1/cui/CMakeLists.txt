cmake_minimum_required(VERSION 3.26)
project(task1)

set(CMAKE_CXX_STANDARD 17)

add_executable(task1 main.cpp
  src/pattern.cpp
  src/dfa.h
  src/dfa.cpp
  src/nfa.h
  src/nfa.cpp
  src/regExp.h
  src/regExp.cpp
  src/generateLexer.cpp
  src/regScanner.h
  src/regScanner.cpp
  src/lexer.h
  src/lexer.cpp
  src/util.h
  src/util.cpp
)

target_include_directories(task1 PRIVATE src)