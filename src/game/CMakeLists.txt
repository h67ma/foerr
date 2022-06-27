cmake_minimum_required(VERSION 2.8.12)

project(foerr_game)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../..)

file(GLOB SOURCES *.cpp)

add_compile_options(-g -rdynamic -Wall)

add_executable(foerr ${SOURCES})

target_link_libraries(foerr sfml-graphics sfml-window sfml-system)
