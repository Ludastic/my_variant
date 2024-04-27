cmake_minimum_required(VERSION 3.22)
project(my_variant)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(my_variant main.cpp my_variant.h)

target_compile_options(my_variant
        PRIVATE
        -Werror
        -Wall
        -Wextra
        -Wpedantic
        -Wold-style-cast
        -Wconversion
        -Weffc++
        )