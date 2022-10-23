include(FetchContent)

FetchContent_Declare(
	sfml
	URL https://github.com/SFML/SFML/archive/refs/tags/2.5.1.zip
	URL_HASH SHA256=6124B5FE3D96E7F681F587E2D5B456CD0EC460393DFE46691F1933D6BDE0640B
)

set(SFML_BUILD_NETWORK OFF)
set(BUILD_SHARED_LIBS OFF)
set(SFML_BUILD_EXAMPLES OFF)
set(SFML_BUILD_DOC OFF)
set(SFML_BUILD_TEST_SUITE OFF)

message(STATUS "Fetching SFML...")
FetchContent_MakeAvailable(sfml)

# workaround, will be fixed in 2.6.0
# that vorbis warning will also be fixed btw
target_compile_definitions(sfml-audio PRIVATE _HAS_AUTO_PTR_ETC)

# TODO copy openal.dll from sfml-src/extlibs/bin/{x64/x86} to root dir or somehow add it to PATH
