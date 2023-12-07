# SPDX-License-Identifier: GPL-3.0-only
#
# (c) 2022-2023 h67ma <szycikm@gmail.com>

cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
include(CMakeGitVersion.cmake)

project(foerr
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

set(CPACK_BINARY_STGZ "OFF")
set(CPACK_BINARY_TZ "OFF")

if(WIN32)
	set(CPACK_BINARY_NSIS "ON")
	set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/res/ico/foerr.ico")
	set(CPACK_NSIS_INSTALLED_ICON_NAME "foerr.exe")
	set(CPACK_NSIS_PACKAGE_NAME "FoE: Remains of Remains")
	set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
	set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON")
elseif(UNIX)
	set(CPACK_BINARY_TGZ "ON")
endif()

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "foerr")
set(CPACK_MONOLITHIC_INSTALL "ON")

# ':' not allowed in shortcut name as it will be a file name.
# for some reason these shortcuts remain in "Start Menu" after uninstall
set(CPACK_PACKAGE_EXECUTABLES "foerr" "FoE Remains of Remains")

get_git_version(${GIT_EXECUTABLE} ${CMAKE_SOURCE_DIR})
set(CPACK_PACKAGE_VERSION ${GIT_VERSION})

include(CPack)
