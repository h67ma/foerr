# Submitting changes and code review
This project follows the
[fork and pull model](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/getting-started/about-collaborative-development-models#fork-and-pull-model).

The process:
1. Fork the project to your own account
2. Create a branch in your forked repository
3. Push commit(s) to your branch
4. Open a pull request targeting the `main` branch of the upstream repository
5. Address any comments raised in the review. To update the pull request, don't add new commits with updates;
instead amend your commit(s) and force push.
6. Pull request can be approved by a Contributor (or Maintainer). Once at least one approve is received, a Maintainer
can merge it.

# Coding conventions and style
Code must be cleanly formatted and cannot include any WIP leftovers, such as messy comments, debug prints, etc.

C code formatting is defined in `src/.clang-format`. To format a file in-place:
```
clang-format -i $FILE
```

Alternatively `--dry-run` flag can be added to only print current style issues.

For automatic code style checks see the [pre-commit hook](#pre-commit-hook) section.

Python/shell/CMake code style is currently not enforced.

# License header
All source files must start with a valid license header, which specifies license type and lists contributors with years
in which they modified the file. When making changes to source files, authors should update the license header,
adding/updating their line if necessary.

For automatic license header checks see the [pre-commit hook](#pre-commit-hook) section.

## C files header
```
// SPDX-License-Identifier: GPL-3.0-only
//
// (c) $YEAR Author Name <test@example.com>
// (c) $YEAR-$YEAR Author Name <test@example.com>
```

## Python/shell/CMake files header
```
# SPDX-License-Identifier: GPL-3.0-only
#
# (c) $YEAR Author Name <test@example.com>
# (c) $YEAR-$YEAR Author Name <test@example.com>
```
For shell scripts the interpreter directive line goes before the license header.

# pre-commit hook
Contributors are strongly encouraged to enable a pre-commit hook, which automatically runs a few checks and aborts the
commit if any problems are found. Checks include C code formatting and license header validation. To enable the hook,
run
`toolz/enable-pre-commit.sh`.

Installed hook can be skipped with `git commit --no-verify`.

# Commits
Commits should be atomic. Whenever possible, don't mix two new features in one commit.
When renaming/refactoring a large chunk of code, don't do it alongside other changes - rather split it off into
a separate commit which is only renaming stuff. This way commits are way easier to understand and review.

Commits cannot break compilation, even if they are part of a bigger changeset.
Code **must compile** (and scripts must execute) after every individual commit.
There can't be any half-finished parts. However, if the change is part of a changeset, it doesn't strictly have to make
sense logically - e.g. commit 1/2 could remove some feature which is actually being actively used, commit 2/2 could
implement a similar feature replacing the original one.

## Commit messages
All commits must include a non-empty commit message describing the change and providing other useful information.

Every commit message must be **properly formatted**. Only exceptions are commits made before 10-10-2023.

In general, the message format is loosely based on the one used in
[Linux Kernel](https://www.kernel.org/doc/html/latest/process/submitting-patches.html).

Commit messages must be written in English, including **proper grammar, spelling, punctuation**, etc.
Please make it understandable. Usage of a spell checker is recommended.

Line length limit is **75 characters**. Only exceptions are links and tags.

Use **imperative mood**, e.g. `make ::myMethod() return bananas`, instead of
`I changed ::myMethod() so it returns bananas` or `::myMethod() returns bananas`.

Commit message consists of three parts (described in detail in next sections):
* Title
* Description
* Tags and sign-off

The parts are separated by one empty newline.

### Title
First line of the message is the *title*. It should be short and concisely describe the whole change.
Title must fit in one line.

Title contains a short identifier at the beginning, then a colon (`:`), then the actual title.
The identifier should point to the main part of the repository that is being changed. Identifiers in use:
* `foerr` - most of the changes were done in `src` directory
* `style` - code style refactors with **no functional changes**
* `toolz` - most of the changes were done in `toolz` directory
* `res` - most of the changes were done in `res` or `fonts` directories
* `build` - changes related to the build system (CMake, dependencies)
* `git` - changes related to git (e.g. `.gitignore` modifications)
* `github` - changes related to Github actions
* `docs` - changes related to non-code documentation

It is encouraged to use words such as `add`, `refactor`, `improve`, or `fix` in the title.

Title example: `toolz: refactor material importing logic`

### Description
Description follows *title* (after one empty newline) and should make the bulk of the commit message.
This is the place to explain the change in detail. It should contain information such as:
* What is the motivation for the change
* What improvements does the change make
* How does the code behave now
* How will the code behave after the change
* Are there any side effects or TODOs related to this subject that should be addressed in the future

It is encouraged to separate the description into short paragraphs, each dealing with separate aspect of the change.

If the change is small or trivial, the description doesn't have to be long, but it shouldn't be empty.

#### Links
To add a link to the description, don't paste it straight into the text, as it would often make the line too long.
Instead, leave a reference mark (`[$number]`, e.g. `[1]`) in the text, and paste the link at the end of description.

Example:

```
Description content which wants to link to some site [1] and then
continue on.

[1] https://example.com
```

### Tags and sign-off
This section follows *description* (after one empty newline) and contains specially formatted information.
Each tag takes one line.

Please refer to [Kernel contributing guide](https://www.kernel.org/doc/html/latest/process/submitting-patches.html#using-reported-by-tested-by-reviewed-by-suggested-by-and-fixes) for detailed description of each tag.
TLDR for most common ones:
* `Fixes` is used to point that the commit fixes some other commit.
It's usually used when the title contains the `fix` keyword. Offending commit must be formatted as `%h (\"%s\")`
with a 12 character SHA, e.g.
`Fixes: 1234567890ab ("foerr: sample commit")`
* `Co-developed-by` is used to credit someone who helped prepare the change,
e.g. `Co-developed-by: User Name <test@example.com>`
* `Suggested-by`, `Reported-by` are used for similar purposes as `Co-developed-by`

Last line after all the tags is always the **sign-off**, which can be automatically generated with
`git commit -s`. Example: `Signed-off-by: User Name <test@example.com>`. Sign-off must match commit author.

### Example commit message
```
res: Fix toasters being unpickable

Currently the player can pick up objects such as lamps, fans, etc, but is
unable to pick up toasters, as reported in an user discussion [1]. Fix
that by setting a correct flag in items.json.

[1] https://example.com/some_discussion

Reported-by: Anon <text@example.com>
Fixes: aabbccddeeff ("foerr: Add the ability to pick up items")
Signed-off-by: User <test2@example.com>
```

# Maintainers note
Maintainers reserve the right to have a final say in any decision related to the project. That being said, they must aim
to listen to other contributors and users and ultimately take an action that is best for project goals.
