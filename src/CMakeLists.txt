cmake_minimum_required(VERSION 3.6)
project(src C)

set(CMAKE_C_STANDARD 11)

add_executable(src main.c ukkonen.h ukkonen.c suffix_tree.h suffix_tree.c)