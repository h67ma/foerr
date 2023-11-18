# SPDX-License-Identifier: GPL-3.0-only
#
# (c) 2022-2023 h67ma <szycikm@gmail.com>

include(FetchContent)

FetchContent_Declare(
	sfml
	URL https://github.com/SFML/SFML/archive/refs/tags/2.6.0.zip
	URL_HASH SHA256=cf9535356ab37067e871270fbc5ae19e4102b658d6ea96993e6c9452ddbf99b3
)

set(SFML_BUILD_NETWORK OFF)
set(BUILD_SHARED_LIBS OFF)
set(SFML_BUILD_EXAMPLES OFF)
set(SFML_BUILD_DOC OFF)
set(SFML_BUILD_TEST_SUITE OFF)

message(STATUS "Fetching SFML...")
FetchContent_MakeAvailable(sfml)

# TODO copy openal.dll from sfml-src/extlibs/bin/{x64/x86} to root dir or somehow add it to PATH
