# cmake_minimum_required(VERSION <specify CMake version here>)
project(exam)

set(CMAKE_CXX_STANDARD 11)

include_directories(SYSTEM "/ui")

add_executable(exam main.cpp tools/src/HotkeyLock.cpp)