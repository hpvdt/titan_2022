cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

project("bikePi")

enable_testing()


add_executable(bikeExecutables ./src/bikePi.cpp ./src/boot.cpp)

add_test(testBike bikeExecutables)