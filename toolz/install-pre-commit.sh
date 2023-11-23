#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# (c) 2023 h67ma <szycikm@gmail.com>

PROJECT_ROOT=$(git rev-parse --show-toplevel)
cd $PROJECT_ROOT
ln -sr toolz/pre-commit.sh .git/hooks/pre-commit
