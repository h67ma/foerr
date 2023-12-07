# based on https://github.com/nocnokneo/cmake-git-versioning-example
function(get_git_version GIT_EXECUTABLE SRC_DIR)
	if(GIT_EXECUTABLE)
		execute_process(
			COMMAND ${GIT_EXECUTABLE} describe --tags --dirty --match "v*"
			WORKING_DIRECTORY ${SRC_DIR}
			OUTPUT_VARIABLE GIT_DESCRIBE_VERSION
			RESULT_VARIABLE GIT_DESCRIBE_ERROR_CODE
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		if(NOT GIT_DESCRIBE_ERROR_CODE)
			set(_GIT_VERSION ${GIT_DESCRIBE_VERSION})
		endif()
	endif()

	if(NOT DEFINED _GIT_VERSION)
		set(_GIT_VERSION v0.0.0-unknown)
		message(WARNING "Failed to determine GIT_VERSION. Using default version: \"${_GIT_VERSION}\".")
	endif()

	set(GIT_VERSION ${_GIT_VERSION} PARENT_SCOPE)
endfunction()

if(DEFINED SRC)
	get_filename_component(SRC_DIR ${SRC} DIRECTORY)
	get_git_version(${GIT_EXECUTABLE} ${SRC_DIR})
	configure_file(${SRC} ${DST} @ONLY)
endif()
