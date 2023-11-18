# SPDX-License-Identifier: GPL-3.0-only
#
# (c) 2022 h67ma <szycikm@gmail.com>

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

add_subdirectory(deps/sfml)
add_subdirectory(deps/nlohmann_json)
add_subdirectory(src)
