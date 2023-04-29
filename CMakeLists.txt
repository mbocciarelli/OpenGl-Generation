cmake_minimum_required(VERSION 3.24)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(opengl-generationterrain)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Game)