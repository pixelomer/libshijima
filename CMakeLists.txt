cmake_minimum_required(VERSION 3.14)
project(shijima)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED SHIJIMA_USE_PUGIXML)
    set(SHIJIMA_USE_PUGIXML YES)
endif()

if(SHIJIMA_USE_PUGIXML)
    add_subdirectory(pugixml)
endif()
include_directories(.)
include_directories(cereal/include)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(NOT DEFINED SHIJIMA_BUILD_EXAMPLES)
    set(SHIJIMA_BUILD_EXAMPLES YES)
endif()

if(CMAKE_BUILD_TYPE EQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DSHIJIMA_DUK_STATIC_BUILD")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

if(NOT SHIJIMA_USE_PUGIXML)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSHIJIMA_NO_PUGIXML")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic")

add_library(
    shijima
    shijima/action/animate.cc
    shijima/action/animation.cc
    shijima/action/base.cc
    shijima/action/breed.cc
    shijima/action/dragged.cc
    shijima/action/fall.cc
    shijima/action/instant.cc
    shijima/action/interact.cc
    shijima/action/jump.cc
    shijima/action/look.cc
    shijima/action/move.cc
    shijima/action/movewithturn.cc
    shijima/action/offset.cc
    shijima/action/reference.cc
    shijima/action/resist.cc
    shijima/action/scanmove.cc
    shijima/action/select.cc
    shijima/action/selfdestruct.cc
    shijima/action/sequence.cc
    shijima/action/stay.cc
    shijima/action/transform.cc
    shijima/action/turn.cc
    shijima/behavior/base.cc
    shijima/behavior/list.cc
    shijima/behavior/manager.cc
    shijima/broadcast/client.cc
    shijima/broadcast/interaction.cc
    shijima/broadcast/manager.cc
    shijima/broadcast/server.cc
    shijima/mascot/factory.cc
    shijima/mascot/manager.cc
    shijima/mascot/state.cc
    shijima/scripting/duktape/duktape.cc
    shijima/scripting/context.cc
    shijima/scripting/variables.cc
    shijima/animation.cc
    shijima/hotspot.cc
    shijima/log.cc
    shijima/parser.cc
    shijima/translator.cc
)

if(SHIJIMA_USE_PUGIXML)
    add_dependencies(shijima pugixml)
    target_link_libraries(shijima pugixml)
endif()

if(SHIJIMA_BUILD_EXAMPLES)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
    
    add_executable(shijima-sandbox main.cc)
    add_dependencies(shijima-sandbox shijima)
    target_link_libraries(shijima-sandbox shijima SDL2_image::SDL2_image ${SDL2_LIBRARIES})

    add_custom_command(TARGET shijima-sandbox POST_BUILD
        COMMAND "$<$<CONFIG:Release>:${CMAKE_STRIP};shijima-sandbox>"
        COMMAND_EXPAND_LISTS)
endif()
