cmake_minimum_required(VERSION 3.14)
project(ex1 C)

link_directories(libmtm/win64)
add_executable(ex1_part2 matamazom.c amount_set.c)
add_executable(Test tests/matamazom_tests.c tests/matamazom_main.c)
target_link_libraries(ex1_part2 libmtm.a)
target_link_libraries(Test libmtm.a)
