# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
cmake_minimum_required(VERSION 3.20)

project (S8-53M2)

set(CMAKE_UNITY_BUILD ON)

set (TARGET_NAME S8-53M2)

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    add_definitions(-DDEBUG)
endif()

if(WIN32)

else()
    add_definitions(-m64 -fmax-errors=1 -Wall)
endif()

add_subdirectory(Device)
add_subdirectory(Loader)
add_subdirectory(Panel)
