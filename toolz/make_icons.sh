#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# (c) 2023 h67ma <szycikm@gmail.com>

# generates ico files from svgs
# run from toolz dir
# svgs must be in res/ico dir

for FILE in ../res/ico/*.svg; do
	NAME="$(basename $FILE .svg)"

	for SIZE in 16 32 48 64; do
		inkscape -w $SIZE -h $SIZE -o "${NAME}_${SIZE}.png" $FILE
	done

	icotool -c -o "../res/ico/${NAME}.ico" "${NAME}"_{16,32,48,64}.png
	rm -f "${NAME}"_{16,32,48,64}.png
done
