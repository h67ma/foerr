include(FetchContent)

FetchContent_Declare(
	jsoncpp
	URL https://github.com/open-source-parsers/jsoncpp/archive/refs/tags/1.9.5.zip
	URL_HASH SHA256=A074E1B38083484E8E07789FD683599D19DA8BB960959C83751CD0284BDF2043
)

set(JSONCPP_WITH_TESTS OFF)
set(BUILD_SHARED_LIBS OFF)

message(STATUS "Fetching jsoncpp...")
FetchContent_MakeAvailable(jsoncpp)
