cmake_minimum_required(VERSION 3.14)
project(shijima)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(.)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(CMAKE_BUILD_TYPE EQUAL "Release")
    set(CMAKE_CXX_FLAGS "-O3 -lto")
else()
    set(CMAKE_CXX_FLAGS "-g -DDEBUG")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror=sign-compare -Werror=return-type")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_library(
    shijima
    shijima/scripting/context.cc
    shijima/scripting/duktape/duktape.cc
    shijima/translator.cc
    shijima/parser.cc
    shijima/log.cc
)

add_executable(shijima-sandbox main.cc)
target_link_libraries(shijima-sandbox shijima SDL2_image::SDL2_image ${SDL2_LIBRARIES})