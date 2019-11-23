cmake_minimum_required(VERSION 3.14)
project(ex1 C)

add_executable(ex1_part2 matamazom.c main.c amount_set.c)
set(CMAKE_C_STANDARD 99)
find_library(MTM_LIB libmtm.a lib)
target_link_libraries(ex1_part2 LINK_PUBLIC ${MTM_LIB})
