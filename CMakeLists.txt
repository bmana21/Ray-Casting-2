cmake_minimum_required(VERSION 3.25)

project(Ray_Casting__2)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Ray_Casting__2 main.cpp)

target_link_libraries(Ray_Casting__2 ${SDL2_LIBRARIES})
