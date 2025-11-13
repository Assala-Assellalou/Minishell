# Minishell

Minishell is a small Unix-like command-line shell implemented as a project (specification at `projet-minishell.pdf`).  
This repository contains an educational implementation of a shell with basic parsing, execution, builtins and redirections — a compact and portable shell used for learning the internals of process control, parsing, and Unix I/O.

> Note: This README is intended to be an overview and a practical guide to build, run and test the project. For the official project specification see `projet-minishell.pdf`.

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


