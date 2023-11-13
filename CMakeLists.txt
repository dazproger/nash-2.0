cmake_minimum_required(VERSION 3.26)
project(nash_equilibrium)

find_package(ortools CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -Wall)

add_executable(sat-test sat-test.cpp)
target_link_libraries(sat-test ortools::ortools)

# FILE(GLOB Sources *.cpp)

add_executable(nash_equilibrium main.cpp game.cpp)
add_executable(nash_equilibrium_asan main.cpp game.cpp)

# target_link_libraries(nash_equilibrium ortools::ortools)
# target_link_libraries(nash_equilibrium_asan ortools::ortools)

set_target_properties(nash_equilibrium_asan PROPERTIES COMPILE_FLAGS "-g -fsanitize=address,undefined")
set_target_properties(nash_equilibrium_asan PROPERTIES LD_FLAGS "-fsanitize=address,undefined")
 