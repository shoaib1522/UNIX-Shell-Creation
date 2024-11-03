
# UNIX-Shell-Creation

## Overview
This project involves creating a custom UNIX shell as part of the Operating Systems Lab. The shell will gradually be developed through various versions, adding features like command parsing, I/O redirection, background process handling, command history, and built-in commands. This shell provides an interactive way for users to execute commands within a UNIX environment, offering both basic and advanced functionalities.

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

### Version 04 (Command History)
- Maintains a history of the last 10 commands entered.
- Typing `history` displays the list of recent commands.
- Allows executing a previous command by typing `!number`, where `number` is the history command index.
  - Example: `!2` executes the second command in history.

### Version 05 (Built-In Commands)
- Implements several built-in commands that the shell handles internally:
  - **`cd <directory>`**: Changes the current working directory.
  - **`exit`**: Exits the shell.
  - **`jobs`**: Lists currently running background processes.
  - **`kill <pid>`**: Terminates a background process by its process ID.
  - **`help`**: Lists all available built-in commands and their syntax.
- These commands do not use `fork()` or `execvp()` since they are handled directly by the shell.

## How to Run
1. Clone the repository.
2. Compile the source code using:
   ```bash
   gcc -o myshell myshell.c
   ```
3. Run the shell:
   ```bash
   ./myshell
   ```

## Usage
- **Basic commands**: Type any UNIX command (e.g., `ls`, `pwd`, `cat`) and press Enter.
- **I/O Redirection**:
  - For input redirection, use `<` followed by the filename (e.g., `sort < data.txt`).
  - For output redirection, use `>` followed by the filename (e.g., `ls > output.txt`).
- **Background processes**: To run a command in the background, add `&` at the end (e.g., `sleep 10 &`).
- **Command History**:
  - Type `history` to see a list of recent commands.
  - Type `!number` to execute a specific command from the history, where `number` is the command index.
- **Built-In Commands**:
  - `cd <directory>`: Change the current directory.
  - `exit`: Exit the shell.
  - `jobs`: Display a list of active background processes.
  - `kill <pid>`: Terminate a background process by its PID.
  - `help`: Display a list of available built-in commands.
- **Exit the Shell**: Type `exit` or press `<CTRL+D>` to exit.

## Current Status
- **Version 01** is implemented with command parsing and basic execution.
- **Version 02** partially implemented with I/O redirection; pipes are planned.
- **Version 03** is partially implemented with background processes; further enhancements planned for handling background jobs.
- **Version 04** is implemented with basic command history and execution of past commands.
- **Version 05** is implemented with built-in commands for `cd`, `exit`, `jobs`, `kill`, and `help`.


### Total Versions for the Assignment:

As per the assignment document, there are **six versions** in total:

1. **Version 01**: Basic Shell with command parsing and execution.
2. **Version 02**: I/O Redirection (input and output redirection, upcoming pipes).
3. **Version 03**: Background processes handling with  `&`.
4. **Version 04**: Command history with `history` display and `!number` execution.
5. **Version 05**: Built-in commands (  like `cd`, `exit`, `jobs`, `kill`, and `help`).
6. **Version 06 (Bonus)**: Variable handling for user-defined and environment variables.

Here's a step-by-step guide to test each version of your UNIX shell implementation, with specific commands for each version.

---

### Version 01: Basic Shell

1. **Run Basic Commands**:
   - `ls` – Lists files and directories in the current directory.
   - `pwd` – Prints the current working directory.
   - `whoami` – Displays the current user.

2. **Exit the Shell**:
   - Type `exit` to exit the shell.
   - Alternatively, press `<CTRL+D>`.

**Expected Outcome**: The shell executes each command, displays output, and exits when `exit` or `<CTRL+D>` is used.

---

### Version 02: I/O Redirection

1. **Output Redirection (`>`)**:
   - Command: `ls > output.txt`
   - Check: Open `output.txt` to see the `ls` output.

2. **Input Redirection (`<`)**:
   - Create a file `unsorted.txt` with unsorted text.
   - Command: `sort < unsorted.txt`
   - Expected Outcome: Displays sorted text in the shell.

3. **Combined Input and Output Redirection**:
   - Command: `sort < unsorted.txt > sorted.txt`
   - Check: Open `sorted.txt` to see the sorted output.

**Expected Outcome**: Commands should correctly use the specified input and output files.

---

### Version 03: Background Processes

1. **Run a Background Process**:
   - Command: `sleep 30 &`
   - Expected Outcome: The shell returns immediately with a message indicating the background process ID.

2. **Check Background Jobs**:
   - Command: `jobs`
   - Expected Outcome: Displays the background process (e.g., `sleep 30`).

3. **Kill a Background Process**:
   - Command: `kill <pid>` (replace `<pid>` with the actual process ID from `jobs`).
   - Verify with `jobs` that the process is terminated.

**Expected Outcome**: Background jobs are listed with `jobs`, and processes can be terminated with `kill`.

---

### Version 04: Command History

1. **Run Multiple Commands**:
   - Examples: `ls`, `pwd`, `whoami`.

2. **View Command History**:
   - Command: `history`
   - Expected Outcome: Lists the last 10 commands with numbers.

3. **Execute Command from History**:
   - Command: `!1` (replace `1` with the number of any command from `history`).
   - Expected Outcome: Repeats the selected command from history.

**Expected Outcome**: The `history` command shows recent commands, and `!number` repeats a specific command.

---

### Version 05: Built-In Commands

1. **Change Directory**:
   - Command: `cd /tmp`
   - Verify with `pwd` that the directory is now `/tmp`.

2. **View Background Jobs**:
   - Start a background job: `sleep 30 &`
   - Command: `jobs`
   - Expected Outcome: Lists the background job.

3. **Kill a Background Job**:
   - Command: `kill <pid>` (use the PID from `jobs`).

4. **Help**:
   - Command: `help`
   - Expected Outcome: Lists all built-in commands and descriptions.

**Expected Outcome**: Built-in commands (`cd`, `jobs`, `kill`, `help`) work as specified.

---

### Version 06: Shell Variables

1. **Set a Variable**:
   - Command: `set myvar=hello`
   - Expected Outcome: The variable `myvar` is set with the value `hello`.

2. **Display Variable**:
   - Command: `myvar`
   - Expected Outcome: Prints `hello`.

3. **Unset Variable**:
   - Command: `unset myvar`
   - Verify by typing `myvar` again; it should give an error or not display anything.

4. **List All Variables**:
   - Command: `printenv`
   - Expected Outcome: Lists all set shell variables.

**Expected Outcome**: Variables can be set, displayed, unset, and listed with `printenv`.

---
## Acknowledgments
- Based on the Operating Systems Lab requirements from PUCIT.
- Thanks to online UNIX programming resources for `fork()`, `execvp()`, `dup2()`, and signal handling with `waitpid()`.

## License
This project is licensed under the MIT License.

