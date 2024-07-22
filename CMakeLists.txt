cmake_minimum_required(VERSION 3.14)
project(shijima)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(.)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(NOT DEFINED SHIJIMA_BUILD_EXAMPLES)
    set(SHIJIMA_BUILD_EXAMPLES YES)
endif()

if(CMAKE_BUILD_TYPE EQUAL "Release")
    set(CMAKE_CXX_FLAGS "-O3 -DSHIJIMA_DUK_STATIC_BUILD")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    set(CMAKE_CXX_FLAGS "-g")
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

if(SHIJIMA_BUILD_EXAMPLES)
    add_executable(shijima-sandbox main.cc)
    target_link_libraries(shijima-sandbox shijima SDL2_image::SDL2_image ${SDL2_LIBRARIES})

    add_custom_command(TARGET shijima-sandbox POST_BUILD
        COMMAND "$<$<CONFIG:Release>:${CMAKE_STRIP};shijima-sandbox>"
        COMMAND_EXPAND_LISTS)
endif()