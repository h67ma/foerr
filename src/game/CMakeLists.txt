file(GLOB_RECURSE SOURCES *.cpp)

include_directories(${PROJECT_SOURCE_DIR}/campaigns)
include_directories(${PROJECT_SOURCE_DIR}/entities)
include_directories(${PROJECT_SOURCE_DIR}/hud)
include_directories(${PROJECT_SOURCE_DIR}/settings)
include_directories(${PROJECT_SOURCE_DIR}/window)

# TODO? for stack trace
#add_compile_options(-g -rdynamic -Wall)

add_executable(foerr ${SOURCES})

if(MSVC)
	# set a tolerable warning level
	add_compile_options(foerr PRIVATE /W4 /WX)
else()
	add_compile_options(foerr PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_link_libraries(foerr PRIVATE sfml-graphics sfml-window sfml-system sfml-audio jsoncpp_static)

install(TARGETS foerr DESTINATION foerr)
install(DIRECTORY "${PROJECT_BINARY_DIR}/../res" DESTINATION foerr)
# TODO? on binbows also copy openal to install dir
