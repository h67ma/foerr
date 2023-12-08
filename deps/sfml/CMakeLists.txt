# SPDX-License-Identifier: GPL-3.0-only
#
# (c) 2022-2023 h67ma <szycikm@gmail.com>

include(FetchContent)

FetchContent_Declare(
	sfml
	URL https://github.com/SFML/SFML/archive/refs/tags/2.6.1.zip
	URL_HASH SHA256=d7c4c524b4224dbc31f802a270f62503a3c37540897de06290d856a74d6faf7c
)

set(SFML_BUILD_NETWORK OFF)
set(BUILD_SHARED_LIBS OFF)
set(SFML_BUILD_EXAMPLES OFF)
set(SFML_BUILD_DOC OFF)
set(SFML_BUILD_TEST_SUITE OFF)

message(STATUS "Fetching SFML...")
FetchContent_MakeAvailable(sfml)

if(WIN32)
	# /EHsc is to somehow mitigate warning C4530 (being treated as an error)
	target_compile_options(sfml-audio PRIVATE /EHsc)
	target_compile_options(sfml-graphics PRIVATE /EHsc)
	target_compile_options(sfml-system PRIVATE /EHsc)
	target_compile_options(sfml-window PRIVATE /EHsc)
endif()
