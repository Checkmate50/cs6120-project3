cmake_minimum_required(VERSION 3.1)
project(Posit)

find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_subdirectory(posit)  # Use your pass name here.
add_subdirectory(float)
