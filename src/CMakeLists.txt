# SPDX-License-Identifier: GPL-3.0-only
#
# (c) 2022 h67ma <szycikm@gmail.com>

file(GLOB_RECURSE SOURCES *.cpp)

add_executable(foerr ${SOURCES})

if(WIN32)
	# set a tolerable warning level
	add_compile_options(foerr PRIVATE /W4 /WX)
elseif(UNIX)
	add_compile_options(foerr PRIVATE -Wall -Wextra -Wpedantic -Werror)
else()
	message(WARNING "Building on unsupported configuration.")
endif()

target_link_libraries(foerr PRIVATE sfml-graphics sfml-window sfml-system sfml-audio nlohmann_json)

install(TARGETS foerr DESTINATION foerr)
install(DIRECTORY "${PROJECT_BINARY_DIR}/../res" DESTINATION foerr)
# TODO? on binbows also copy openal to install dir

# add version from git-describe
# based on https://github.com/nocnokneo/cmake-git-versioning-example
find_package(Git)
add_custom_target(gitversion
	${CMAKE_COMMAND} -D SRC=${CMAKE_SOURCE_DIR}/src/git_version.h.in
					 -D DST=${CMAKE_BINARY_DIR}/include/git_version.h
					 -D GIT_EXECUTABLE=${GIT_EXECUTABLE}
					 -P ${CMAKE_SOURCE_DIR}/CMakeGitVersion.cmake
)
target_include_directories(foerr PRIVATE ${CMAKE_BINARY_DIR}/include)
add_dependencies(foerr gitversion)
