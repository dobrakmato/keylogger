cmake_minimum_required(VERSION 3.8)
project(keystroke_counter)

set(CMAKE_C_STANDARD 11)
find_library(PSAPI Psapi)

set(SOURCE_FILES main.c)
add_executable(keystroke_counter ${SOURCE_FILES})
target_link_libraries(keystroke_counter psapi)