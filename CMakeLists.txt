cmake_minimum_required(VERSION 3.10)
project(ex1 C)
set(${CMAKE_C_FLAGS} -std=c99 -g -Wall -pedantic-errors -Werror)
#link_directories(libmtm/win32)
link_directories(libmtm/win32)
#link_directories(tests_by_people)
#add_executable(ex1_part2 matamazom.c amount_set.c matamazom_print.c)
add_executable(Test tests/matamazom_tests.c tests/test_matamazom_yonatan_and_hen.c matamazom.c amount_set.c matamazom_print.c)
add_executable(TestByPeople main.c matamazom.c amount_set.c matamazom_print.c)
#add_executable(TestByYonatan main.c matamazom.c amount_set.c matamazom_print.c)
#add_executable(matamazom_by_noa matamazom_tests_by_noa.c matamazom.c amount_set.c matamazom_print.c)
#add_executable(test_by_noa test_by_noa.c amount_set.c)
#add_executable(ex1_part1 amount_set.c tests/amount_set_main.c tests/amount_set_tests.c)
#add_executable(tests_by_safi main_safi.c amount_set.c)

target_link_libraries(Test libmtm.a)
#target_link_libraries(TestByYonatan libmtm.a)
target_link_libraries(TestByPeople libmtm.a)
#target_link_libraries(matamazom_by_noa libmtm.a)