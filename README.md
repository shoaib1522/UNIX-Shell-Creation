# UNIX-Shell-Creation

## Overview
This project involves creating a custom UNIX shell as part of the Operating Systems Lab. The shell will gradually be developed through various versions, adding features like command parsing, I/O redirection, background process handling, and command history. The shell allows users to interact with the underlying UNIX system through commands and provides several built-in functionalities.

## Versions & Features

### Version 01 (Basic Shell)
- Displays a prompt: `PUCITshell:-`.
- Allows the user to input commands with arguments.
- Uses `fork()` to create a new process and `exec()` to execute commands.
- The parent process waits for the child process to complete.
- Allows exiting the shell with `<CTRL+D>`.

