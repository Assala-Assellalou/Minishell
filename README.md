# Minishell

Minishell is a small Unix-like command-line shell implemented as a project (specification at `projet-minishell.pdf`).  
This repository contains an educational implementation of a shell with basic parsing, execution, builtins and redirections — a compact and portable shell used for learning the internals of process control, parsing, and Unix I/O.

> Note: This README is intended to be an overview and a practical guide to build, run and test the project. For the official project specification see `projet-minishell.pdf`.

## Table of contents
- [Features](#features)
- [Requirements](#requirements)
- [Build](#build)
- [Usage](#usage)
- [Supported builtins](#supported-builtins)
- [Shell behavior and supported syntax](#shell-behavior-and-supported-syntax)
- [Implementation notes](#implementation-notes)
- [Testing and debugging tips](#testing-and-debugging-tips)
- [Known limitations](#known-limitations)
- [Contributing](#contributing)
- [License & Credits](#license--credits)

## Features
- Read-eval loop with a prompt
- Tokenization / lexical analysis of command lines
- Quote handling (single `'` and double `"` quotes)
- Variable expansion using environment variables (e.g., `$HOME`, `$PATH`)
- Execution of commands found in `PATH` using `fork`/`execve`
- Support for simple pipelines (e.g., `ls | grep src`)
- Input and output redirections (`>`, `>>`, `<`)
- Here-document support (`<<`)
- Basic builtin commands (see list below)
- Proper handling of child processes and exit statuses
- Signal handling for interactive use (Ctrl-C, Ctrl-\, etc.)

## Requirements
- Unix-like OS (Linux, macOS)
- POSIX-compatible shell utilities for testing
- C compiler (e.g., `gcc`)
- Make (optional, if Makefile provided)
- Recommended: valgrind to check for memory leaks while debugging

## Build
Typical build steps — adjust to your project's Makefile or build system.

1. Clone the repository:
   git clone https://github.com/Assala-Assellalou/Minishell.git
2. Enter the project directory:
   cd Minishell
3. Build (if a Makefile exists):
   make

If no Makefile is provided, compile manually:
   gcc -Wall -Wextra -Werror -g -o minishell src/*.c

Replace `src/*.c` with the actual source file locations used in this repo.

## Usage
Run the shell executable:
   ./minishell

Example session:
   $ ./minishell
   minishell$ echo "Hello world"
   Hello world
   minishell$ ls -la | grep README
   -rw-r--r--  1 user  staff   1.2K Nov 13  README.md
   minishell$ cat << EOF
   > line1
   > line2
   > EOF
   line1
   line2
   minishell$ exit

Notes:
- Commands typed are executed by searching `PATH`.
- Builtins are executed inside the shell process when required (so they can modify the shell state).

## Supported builtins
This project typically implements the following builtins (confirm against your source):
- `echo` — print arguments
- `cd` — change current directory
- `pwd` — print working directory
- `export` — set environment variables
- `unset` — remove environment variables
- `env` — display environment variables
- `exit` — exit the shell

Behavior should follow the project specification (edge cases, return values).

## Shell behavior and supported syntax
- Word splitting by whitespace
- Single quotes `'...'` — preserve literal text (no expansions)
- Double quotes `"...”` — allow expansions but prevent word splitting
- Environment variable expansion `$VAR` and `$?` (exit status)
- Redirections:
  - `>` — redirect stdout (truncate)
  - `>>` — redirect stdout (append)
  - `<` — redirect stdin from file
  - `<<` — here-document (interactive multiline input until delimiter)
- Pipes: `cmd1 | cmd2 | cmd3` — standard Unix pipeline semantics
- Proper close of file descriptors and cleanup of child processes

## Implementation notes
- Lexing/Parsing: input is first tokenized, handling quotes and escaping; then a parsing stage builds commands and redirection lists.
- Execution: for pipelines, the shell forks processes, sets up pipes and redirects, then calls `execve`.
- Builtins: executed in the parent shell process when they affect shell state (e.g., `cd`, `export`, `exit`), or in children when used in pipelines depending on the project constraints.
- Signals: shell ignores or handles interactive signals (e.g., Ctrl-C) appropriately; child processes may restore default handling.
- Memory: ensure allocated memory is freed on exit; recommended to validate with `valgrind`.

## Testing and debugging tips
- Test basic command execution: `ls`, `echo`, `sleep`
- Test quoting and expansions:
  - echo "This is $HOME"
  - echo 'This is $HOME'
- Test redirections:
  - echo hi > /tmp/testfile
  - cat < /tmp/testfile
- Test pipelines:
  - ps aux | grep minishell | wc -l
- Test here-document:
  cat << EOF
  one
  two
  EOF
- Use Valgrind to find leaks:
  valgrind --leak-check=full ./minishell
- Use `strace` (Linux) or `dtruss`/`ktrace` for system call tracing if needed.

## Known limitations
- Not all advanced shell features are implemented (job control, background `&`, complex expansions, advanced globbing).
- Behavior may differ from `/bin/bash` on edge cases; refer to the PDF spec for required behavior.

## Contributing
This repository is intended as a student learning project. If you intend to contribute:
- Fork the repo
- Create feature branches for large additions
- Ensure no leak is introduced, and maintain tidy code and comments
- Add tests for behavior you add or change

If you are the project owner (Assala-Assellalou), ensure the README reflects any repository-specific build scripts, directories (src, includes, tests), and exact usage.

## License & Credits
- This is an educational project — please check your institution's policies and license choice.
- Credits: project based on the Minishell assignment specification (see `projet-minishell.pdf` in the repository).

If you want, I can:
- Tailor this README to the exact directory layout and Makefile of this repo (I can read the repo and generate a README with precise build commands).
- Add badges (build status, valgrind) once CI or scripts exist.
- Translate or adapt the README to French to match the project PDF language.
