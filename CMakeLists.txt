cmake_minimum_required(VERSION 3.22.1)
project(nash-2.0)

find_package(ortools CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS -Wall)

# Указываем, что заголовки и библиотека находятся в стандартных путях
find_library(NAUTY_LIB nauty.a REQUIRED)
find_path(NAUTY_INCLUDE_DIR nauty.h REQUIRED)

# Проверяем, что Nauty найден
if(NOT NAUTY_LIB OR NOT NAUTY_INCLUDE_DIR)
    message(FATAL_ERROR "Nauty library not found!")
endif()

# Указываем пути для компиляции
include_directories(${NAUTY_INCLUDE_DIR})

# Добавляем исполняемые файлы
add_executable(sat-test sat-test.cpp)
target_link_libraries(sat-test ortools::ortools)

add_executable(nash-2.0 main.cpp game.cpp sat.cpp checker.cpp graph-iter.cpp)
add_executable(nash-2.0_asan main.cpp game.cpp checker.cpp graph-iter.cpp)

# Линкуем библиотеки
target_link_libraries(nash-2.0 ortools::ortools ${NAUTY_LIB})
target_link_libraries(nash-2.0_asan ortools::ortools ${NAUTY_LIB})

# Настройки для AddressSanitizer
set_target_properties(nash-2.0_asan PROPERTIES COMPILE_FLAGS "-g -fsanitize=address,undefined")
set_target_properties(nash-2.0_asan PROPERTIES LD_FLAGS "-fsanitize=address,undefined")
