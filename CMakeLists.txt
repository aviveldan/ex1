cmake_minimum_required(VERSION 3.14)
project(ex1 C)

set(CMAKE_C_STANDARD 99)

add_executable(ex1 main.c tests/amount_set_main.c tests/amount_set_tests.c tests/amount_set_tests.h amount_set.h amount_set.c tests/test_utilities.h matamazom.c)