cmake_minimum_required(VERSION 3.14)
project(shijima)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(.)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(CMAKE_BUILD_TYPE EQUAL "Release")
    set(CMAKE_CXX_FLAGS "-O3 -Wall -Werror=return-type")
else()
    set(CMAKE_CXX_FLAGS "-g -Wall -Werror=return-type")
endif()

add_library(
    libshijima
    shijima/action/animate.cc
    shijima/action/dragged.cc
    shijima/action/jump.cc
    shijima/action/animation.cc
    shijima/action/base.cc
    shijima/action/fall.cc
    shijima/action/instant.cc
    shijima/action/look.cc
    shijima/action/move.cc
    shijima/action/offset.cc
    shijima/action/reference.cc
    shijima/action/select.cc
    shijima/action/sequence.cc
    shijima/action/stay.cc
    shijima/animation.cc
    shijima/mascot/environment.cc
    shijima/mascot/mascot.cc
    shijima/mascot/state.cc
    shijima/parser.cc
    shijima/pose.cc
    shijima/scripting/context.cc
    shijima/scripting/duktape/duktape.cc
    shijima/scripting/variables.cc
    shijima/variable.cc
)

add_executable(shijima-cli main.cc)
target_link_libraries(shijima-cli libshijima)

if(TEST)
    if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
        cmake_policy(SET CMP0135 NEW)
    endif()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/f8d7d77c06936315286eb55f8de22cd23c188571.zip
    )

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(
        libshijima_test
        tests/parser_test.cc
    )
    target_link_libraries(libshijima_test GTest::gtest_main)
    target_link_libraries(libshijima_test libshijima)

    set_target_properties(libshijima_test
        PROPERTIES
        CXX_STANDARD 14
        CXX_EXTENSIONS off
    )

    include(GoogleTest)
    gtest_discover_tests(libshijima_test)
endif()