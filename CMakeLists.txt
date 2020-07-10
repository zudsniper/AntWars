cmake_minimum_required(VERSION 3.14)
project(AntWars)

set(CMAKE_CXX_STANDARD 11)

set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Wextra -Wuninitialized -pedantic-errors -Wconversion")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

add_executable(AntWars Main.cpp Creature.cpp Creature.h Ant.cpp Ant.h Beetle.cpp Beetle.h)