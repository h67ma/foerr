# SPDX-License-Identifier: GPL-3.0-only
#
# (c) 2022-2023 h67ma <szycikm@gmail.com>

include(FetchContent)

FetchContent_Declare(
	nlohmann_json
	URL https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.zip
	URL_HASH SHA256=04022b05d806eb5ff73023c280b68697d12b93e1b7267a0b22a1a39ec7578069
)

set(JSON_BuildTests OFF)
set(JSON_CI OFF)

message(STATUS "Fetching JSON for Modern C++...")
FetchContent_MakeAvailable(nlohmann_json)
