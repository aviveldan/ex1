cmake_minimum_required(VERSION 3.14)
project(ex1 C)

set(CMAKE_C_STANDARD 99)

add_executable(ex1_part2 matamazom.c main.c amount_set.c amount_set.h list.h libmtm.a)