cmake_minimum_required(VERSION 3.20)
project(BlockBlaster
    VERSION 1.0.0
    DESCRIPTION "BlockBlaster is a classic breakout-style game built using Raylib"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0 
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

add_executable(BlockBlaster src/main.c)

target_include_directories(BlockBlaster PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(BlockBlaster PRIVATE raylib)

add_custom_command(TARGET BlockBlaster POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:BlockBlaster>/assets"
)
