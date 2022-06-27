cmake_minimum_required(VERSION 2.8.12)

project(foerr_game)

file(GLOB_RECURSE SOURCES *.cpp)
include_directories(${PROJECT_SOURCE_DIR}/hud)
include_directories(${PROJECT_SOURCE_DIR}/settings)
include_directories(${PROJECT_SOURCE_DIR}/window)

add_compile_options(-g -rdynamic -Wall)

add_executable(foerr ${SOURCES})

target_link_libraries(foerr sfml-graphics sfml-window sfml-system)

install(TARGETS foerr DESTINATION foerr)
install(DIRECTORY "${PROJECT_BINARY_DIR}/../res" DESTINATION foerr)
