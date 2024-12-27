cmake_minimum_required(VERSION 3.22.1)
project(nash-2.0)

find_package(ortools CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS -Wall)

add_executable(sat-test sat-test.cpp)
target_link_libraries(sat-test ortools::ortools)

# FILE(GLOB Sources *.cpp)

add_executable(nash-2.0 main.cpp game.cpp sat.cpp)
add_executable(nash-2.0_asan main.cpp game.cpp)

target_link_libraries(nash-2.0 ortools::ortools)
# target_link_libraries(nash_equilibrium_asan ortools::ortools)

set_target_properties(nash-2.0_asan PROPERTIES COMPILE_FLAGS "-g -fsanitize=address,undefined")
set_target_properties(nash-2.0_asan PROPERTIES LD_FLAGS "-fsanitize=address,undefined")
 
