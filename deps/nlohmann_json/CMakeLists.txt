include(FetchContent)

FetchContent_Declare(
	nlohmann_json
	URL https://github.com/nlohmann/json/archive/refs/tags/v3.11.2.zip
	URL_HASH SHA256=95651D7D1FCF2E5C3163C3D37DF6D6B3E9E5027299E6BD050D157322CEDA9AC9
)

set(JSON_BuildTests OFF)

message(STATUS "Fetching JSON for Modern C++...")
FetchContent_MakeAvailable(nlohmann_json)
