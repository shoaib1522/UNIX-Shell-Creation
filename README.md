# UNIX-Shell-Creation

## Overview
This project involves creating a custom UNIX shell as part of the Operating Systems Lab. The shell will gradually be developed through various versions, adding features like command parsing, I/O redirection, background process handling, and command history. This shell provides an interactive way for users to execute commands within a UNIX environment, offering both basic and advanced functionalities.

## Versions & Features

### Version 01 (Basic Shell)
- Displays a prompt: `PUCITshell:-`.
- Allows the user to input commands with arguments.
- Uses `fork()` to create a new process and `execvp()` to execute commands.
- The parent process waits for the child process to complete.
- Allows exiting the shell with `<CTRL+D>`.

### Version 02 (I/O Redirection)
- Supports input (`<`) and output (`>`) redirection.
- Example usage:
  - `mycmd < infile > outfile` runs `mycmd` with input from `infile` and directs output to `outfile`.
  - Uses `open()` and `dup2()` to handle redirection.
- Basic error handling for file operations.
- (Upcoming) Pipes support for passing the output of one command as input to another, e.g., `cat file.txt | wc`.

### Version 03 (Background Processes)
- Supports running processes in the background using `&` at the end of the command.
  - Example: `find / -name file.txt &` runs the `find` command in the background, returning control to the shell immediately.
- Handles basic signal processing to prevent zombie processes using `waitpid()` with `WNOHANG`.

## How to Run
1. Clone the repository.
2. Compile the source code using:
   ```bash
   gcc -o Shell Shell.c
   ```
3. Run the shell:
   ```bash
   ./Shell
   ```

## Usage
- **Basic commands**: Type any UNIX command (e.g., `ls`, `pwd`, `cat`) and press Enter.
- **I/O Redirection**:
  - For input redirection, use `<` followed by the filename (e.g., `sort < data.txt`).
  - For output redirection, use `>` followed by the filename (e.g., `ls > output.txt`).
- **Background processes**: To run a command in the background, add `&` at the end (e.g., `sleep 10 &`).
- **Exit the Shell**: Type `exit` or press `<CTRL+D>` to exit.

## Current Status
- **Version 01** is implemented with command parsing and basic execution.
- **Version 02** partially implemented with I/O redirection; pipes are planned.
- **Version 03** is partially implemented with background processes; further enhancements planned for handling background jobs.

## Known Bugs
- Argument parsing is limited and does not handle complex commands with pipes yet.
- Error handling for some edge cases in redirection is minimal.
- Background job listing and management (e.g., `jobs` command) are planned but not yet implemented.

## Acknowledgments
- Based on the Operating Systems Lab requirements from PUCIT.
- Thanks to online UNIX programming resources for `fork()`, `execvp()`, `dup2()`, and signal handling with `waitpid()`.

## License
This project is licensed under the MIT License.

