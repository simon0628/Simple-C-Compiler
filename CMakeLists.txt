cmake_minimum_required(VERSION 3.12)
project(compiler)

set(CMAKE_CXX_STANDARD 14)

add_executable(compiler main.cpp Analyze.cpp Lexical.cpp common.cpp Syntax.cpp)