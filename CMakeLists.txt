cmake_minimum_required(VERSION 3.14)
project(shijima)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(.)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(CMAKE_BUILD_TYPE EQUAL "Release")
    set(CMAKE_CXX_FLAGS "-O3 -lto -Wall -Wextra -Werror=sign-compare -Werror=return-type")
else()
    set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Werror=sign-compare -Werror=return-type")
endif()

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_library(
    libshijima
    shijima/scripting/context.cc
    shijima/scripting/duktape/duktape.cc
    shijima/translator.cc
    shijima/parser.cc
)

add_executable(shijima-cli main.cc)
