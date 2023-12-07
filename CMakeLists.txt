# SPDX-License-Identifier: GPL-3.0-only
#
# (c) 2022-2023 h67ma <szycikm@gmail.com>

cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

project(foerr_game
	LANGUAGES CXX
)

# don't warn about DOWNLOAD_EXTRACT_TIMESTAMP in 3.24+
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
	cmake_policy(SET CMP0135 NEW)
endif()

# EXCLUDE_FROM_ALL skips SFML's files from being installed.
# we don't need them, also some of them want to install to system paths and require superuser privileges to do so
add_subdirectory(deps/sfml EXCLUDE_FROM_ALL)

add_subdirectory(deps/nlohmann_json)
add_subdirectory(src)

install(DIRECTORY res DESTINATION ".")
install(DIRECTORY fonts DESTINATION ".")
install(FILES LICENSE DESTINATION ".")

if(WIN32)
	# seems a bit hacky but I guess it works?
	# TODO detect x86/x64 and select dll from proper path
	install(FILES "${PROJECT_BINARY_DIR}/_deps/sfml-src/extlibs/bin/x64/openal32.dll" DESTINATION ".")
endif()
