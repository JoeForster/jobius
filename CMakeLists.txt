cmake_minimum_required (VERSION 3.8)

# TODO
#set(CMAKE_GENERATOR_TOOLSET "ClangCL")

project(Jobius)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

add_subdirectory(src)
