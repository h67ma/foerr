file(GLOB_RECURSE SOURCES *.cpp)

# TODO? for stack trace
#add_compile_options(-g -rdynamic -Wall)

add_executable(foerr ${SOURCES})

if(MSVC)
	# set a tolerable warning level
	add_compile_options(foerr PRIVATE /W4 /WX)
else()
	add_compile_options(foerr PRIVATE -Wall -Wextra -Wpedantic -Werror)
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
