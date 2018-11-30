cmake_minimum_required(VERSION 3.6)
project(src C)

set(CMAKE_C_STANDARD 11)

add_executable(lz77 lz77/main.c lz77/lz77.c ukkonen/ukkonen.c)
add_executable(ukkonen ukkonen/main.c ukkonen/ukkonen.c)
add_executable(test-lz77 ../tests/lz77/main.c lz77/lz77.c ukkonen/ukkonen.c)
add_executable(test-ukkonen ../tests/ukkonen/main.c ukkonen/ukkonen.c ../tests/util.c)