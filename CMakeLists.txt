cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)
project(nash-2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -O0 -g3 -fsanitize=undefined,address")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra -std=gnu++20 -g0 -O2")
find_package(ortools CONFIG REQUIRED)

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
add_executable(sat-test src/sat-test.cpp)
target_link_libraries(sat-test ortools::ortools)

set(SOURCES 
    src/game.cpp 
    src/sat.cpp
    src/checker.cpp
    src/graph-iter.cpp
        include/output-colors.h)

add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES})

# Линкуем библиотеки
target_link_libraries(${PROJECT_NAME} ortools::ortools ${NAUTY_LIB})

target_include_directories(${PROJECT_NAME} PRIVATE include/)

