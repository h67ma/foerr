#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# (c) 2023 h67ma <szycikm@gmail.com>

# adds the GPL header to applicable project files. assumes a SINGLE AUTHOR created everything.

append_header()
{
	FILENAME=$1	# input filename
	COMMENT=$2	# comment character
	LINE=$3		# line number to insert the header

	CREATION_YEAR=`git log --diff-filter=A --follow --format=%ad --date=format:%Y --max-count=1 -- $1`
	LAST_COMMIT_YEAR=`git log --format=%ad --date=format:%Y --max-count=1 -- $1`

	DATES="$COMMENT (c) $CREATION_YEAR"

	if [ "$CREATION_YEAR" != "$LAST_COMMIT_YEAR" ]; then
		DATES=$DATES"-$LAST_COMMIT_YEAR"
	fi

	HEADER="$COMMENT SPDX-License-Identifier: GPL-3.0-only\n$COMMENT\n$DATES $AUTHOR\n\n"

	sed -i "${LINE}s/^/$HEADER/" $1
}

if [ "$#" != "1" ]; then
	echo "Usage: ./append_gpl_headers.sh \"Author Name <author.email@example.com>\""
	exit 1
fi

AUTHOR=$1

DIR_CHECK_CNT=`ls | grep "toolz\|src\|res\|deps" | wc -l`
if [ "$DIR_CHECK_CNT" != "4" ]; then
	echo "Run this script only in root project directory!"
	exit 1
fi

for FILE in $(find src -name "*.hpp" -o -name "*.cpp" -o -name "*.c" -o -name "*.h" -o -name "*.h.in"); do
	append_header $FILE "\/\/" 1
done

append_header src/CMakeLists.txt "#" 1
append_header CMakeLists.txt "#" 1
#append_header CMakeGitVersion.cmake "#" 1

for FILE in $(find deps -name "CMakeLists.txt"); do
	append_header $FILE "#" 1
done

for FILE in $(find toolz -name "*.sh"); do
	append_header $FILE "#" 2
done

for FILE in $(find toolz -name "*.py"); do
	append_header $FILE "#" 1
done
