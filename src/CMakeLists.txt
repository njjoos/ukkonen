cmake_minimum_required(VERSION 3.6)
project(src C)

set(CMAKE_C_STANDARD 11)

add_executable(lz77 lz77/main.c lz77/lz77.h lz77/lz77.c)
add_executable(ukkonen ukkonen/main.c ukkonen/ukkonen.h ukkonen/ukkonen.c)