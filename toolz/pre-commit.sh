#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# (c) 2023 h67ma <szycikm@gmail.com>

# to enable this hook, run install-pre-commit.sh

check_license_header()
{
	EXPECTED_SIGNOFF="$1"
	STAGED_FILE="$2"
	COMMENT_START="$3" # "//" or "#" depending on language
	SHEBANG="$4" # "#!/usr/bin/env bash\n", etc., or empty

	ESCAPED_COMMENT=$(echo "$COMMENT_START" | sed "s/\//\\\\\//g") # beautiful, just beautiful

	# extract license header from staged file
	LICENSE_HEADER=$(
		echo "$STAGED_FILE" |
		grep -Pazo "^$SHEBANG$COMMENT_START SPDX-License-Identifier: GPL-3\.0-only\n$COMMENT_START\n(?:$COMMENT_START \(c\) (?:\d{4}-)?\d{4} [^<]+ <[^>]+>\n)+" |
		tr '\0' '\n'
	)

	# verify the license header exists and is valid
	if [[ -z "$LICENSE_HEADER" ]]; then
		echo "$FILENAME: license header broken or missing, aborting."
		exit 1
	fi

	# verify that the author is listed in the header
	AUTHOR_LINE=$(echo "$LICENSE_HEADER" | grep "$EXPECTED_SIGNOFF")
	if [[ $? -ne 0 ]]; then
		echo "$FILENAME: expected author ($EXPECTED_SIGNOFF) missing from license header, aborting."
		exit 1
	fi

	# verify that the "to" year matches current year
	CURRENT_YEAR=$(date +"%Y")
	HEADER_YEAR=$(echo "$AUTHOR_LINE" | sed -E "s/^$ESCAPED_COMMENT \(c\) ([0-9]{4}-)?([0-9]{4}).+/\2/")
	if [[ "$CURRENT_YEAR" -ne "$HEADER_YEAR" ]]; then
		echo "$FILENAME: incorrect year in license header - expected $CURRENT_YEAR, aborting."
		echo "$AUTHOR_LINE"
		exit 1
	fi
}

EXPECTED_NAME=$(git config --global --get user.name)
EXPECTED_EMAIL=$(git config --global --get user.email)
EXPECTED_SIGNOFF="$EXPECTED_NAME <$EXPECTED_EMAIL>"

CHANGED_FILES=$(git diff --name-only --cached --diff-filter=ACMR)

# we need to enter the directory containing the .clang-format file, since apparently when reading from stdin,
# the config path can't be specified
PROJECT_ROOT=$(git rev-parse --show-toplevel)
cd $PROJECT_ROOT/src
# other than clang-format, working dir has no impact on the rest of this script (as long as it's somewhere in the repo)

C_FILES=$(echo "$CHANGED_FILES" | grep "\.cpp$\|\.hpp$\|\.c$\|\.h$\|\.h\.in$")
for FILENAME in $C_FILES; do
	STAGED_FILE=$(git cat-file -p :$FILENAME)

	check_license_header "$EXPECTED_SIGNOFF" "$STAGED_FILE" "//" ""

	# check code style
	echo "$STAGED_FILE" | clang-format -style=file -Werror --dry-run --assume-filename=$FILENAME
	if [[ $? -ne 0 ]]; then
		echo "Code style errors found, aborting."
		exit 1
	fi

	# TODO? maybe also run clang-tidy?
done

PY_CMAKE_FILES=$(echo "$CHANGED_FILES" | grep "\.py$\|CMakeLists\.txt$")
for FILENAME in $PY_CMAKE_FILES; do
	STAGED_FILE=$(git cat-file -p :$FILENAME)

	check_license_header "$EXPECTED_SIGNOFF" "$STAGED_FILE" "#" ""

	# TODO? figure out python code style checks
done

SH_FILES=$(echo "$CHANGED_FILES" | grep "\.sh$")
for FILENAME in $SH_FILES; do
	STAGED_FILE=$(git cat-file -p :$FILENAME)

	check_license_header "$EXPECTED_SIGNOFF" "$STAGED_FILE" "#" "#!/usr/bin/env bash\n"
done
