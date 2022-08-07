cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_CXX_STANDARD 11)

project(foerr_game)

file(GLOB_RECURSE SOURCES *.cpp)

include_directories(${PROJECT_SOURCE_DIR}/campaigns)
include_directories(${PROJECT_SOURCE_DIR}/entities)
include_directories(${PROJECT_SOURCE_DIR}/hud)
include_directories(${PROJECT_SOURCE_DIR}/settings)
include_directories(${PROJECT_SOURCE_DIR}/window)

find_path(SFML_INCLUDE SFML)
find_path(JSON_INCLUDE json)
include_directories(${SFML_INCLUDE} ${JSON_INCLUDE})

# TODO? for stack trace
#add_compile_options(-g -rdynamic -Wall)

add_executable(foerr ${SOURCES})

find_library(SFML_GRAPHICS_D sfml-graphics-d)
find_library(SFML_WINDOW_D sfml-window-d)
find_library(SFML_SYSTEM_D sfml-system-d)
find_library(SFML_AUDIO_D sfml-audio-d)
find_library(JSON jsoncpp-d)

find_library(SFML_GRAPHICS sfml-graphics)
find_library(SFML_WINDOW sfml-window)
find_library(SFML_SYSTEM sfml-system)
find_library(SFML_AUDIO sfml-audio)
find_library(JSON jsoncpp)

if(MSVC)
	# set a tolerable warning level
	add_compile_options(foerr PRIVATE /W4 /WX)

	if(CMAKE_BUILD_TYPE MATCHES Debug)
		target_link_libraries(foerr ${SFML_GRAPHICS_D} ${SFML_WINDOW_D} ${SFML_SYSTEM_D} ${SFML_AUDIO_D} ${JSON})
	else()
		target_link_libraries(foerr ${SFML_GRAPHICS} ${SFML_WINDOW} ${SFML_SYSTEM} ${SFML_AUDIO} ${JSON})
	endif()
else()
	add_compile_options(foerr PRIVATE -Wall -Wextra -Wpedantic -Werror)
	target_link_libraries(foerr ${SFML_GRAPHICS} ${SFML_WINDOW} ${SFML_SYSTEM} ${SFML_AUDIO} ${JSON})
endif()

install(TARGETS foerr DESTINATION foerr)
install(DIRECTORY "${PROJECT_BINARY_DIR}/../res" DESTINATION foerr)
# TODO? on binbows also install sfml libs to install dir
