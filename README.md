

# UNIX Shell Project

## Overview
This project involves creating a custom UNIX shell for an Operating Systems lab assignment. The shell simulates essential shell features, allowing users to execute commands, manage background processes, use input/output redirection, manage command history, and handle shell variables. Each version of the shell builds upon the previous, gradually adding more functionality until a fully functional shell environment is achieved.

## Features by Version

### Version 01: Basic Shell
- **Objective**: Implement the foundational shell capabilities.
- **Features**:
  - Displays a custom prompt.
  - Parses user input to execute basic commands.
  - Uses `fork()` and `exec()` to execute commands in a child process.
  - Allows the shell to exit using the `exit` command or `<CTRL+D>`.
  
- **Usage**:
  - Run simple commands such as `ls`, `pwd`, `whoami`.
  - Exit by typing `exit` or pressing `<CTRL+D>`.

### Version 02: I/O Redirection
- **Objective**: Add input and output redirection.
- **Features**:
  - Supports output redirection using `>`.
  - Supports input redirection using `<`.
  - Allows combined input and output redirection, e.g., `command < input.txt > output.txt`.
  
- **Usage**:
  - Redirect output: `ls > output.txt` (writes the output of `ls` to `output.txt`).
  - Redirect input: `sort < unsorted.txt` (reads from `unsorted.txt`).
  - Combined redirection: `sort < unsorted.txt > sorted.txt` (sorts contents of `unsorted.txt` and writes to `sorted.txt`).

### Version 03: Background Processes
- **Objective**: Implement background processing.
- **Features**:
  - Enables background process execution by appending `&` to commands.
  - Displays background job details (PID and job ID) when a job is started.
  - Prevents "zombie" processes by handling background processes in a non-blocking way using `waitpid()` with `WNOHANG`.
  
- **Usage**:
  - Start a background job: `sleep 30 &` (runs `sleep` in the background).
  - List background jobs: `jobs`.
  - Kill a background job: `kill <pid>` (replace `<pid>` with the process ID).

### Version 04: Command History
- **Objective**: Provide a command history feature.
- **Features**:
  - Tracks up to the last 10 commands.
  - Displays command history using `history`.
  - Supports re-execution of a specific command from history by typing `!number`, where `number` is the command’s position in the history list.
  
- **Usage**:
  - View history: `history` (lists the last 10 commands).
  - Execute a previous command: `!1` (executes the first command from history).

### Version 05: Built-In Commands
- **Objective**: Implement core built-in commands without forking.
- **Features**:
  - Implements essential built-in commands: `cd`, `jobs`, `kill`, and `help`.
    - `cd <directory>`: Changes the current directory.
    - `jobs`: Lists all active background jobs.
    - `kill <pid>`: Terminates a background process by its PID.
    - `help`: Displays available built-in commands and their usage.
  
- **Usage**:
  - Change directory: `cd /tmp` (moves to `/tmp` directory).
  - View background jobs: `jobs`.
  - Terminate a process: `kill <pid>`.
  - Get help: `help`.

### Version 06: Shell Variables
- **Objective**: Add shell variable support for setting, retrieving, and unsetting variables.
- **Features**:
  - Enables users to define variables with `set <name>=<value>`.
  - Retrieves variable values by typing the variable name directly.
  - Lists all defined shell variables using `printenv`.
  - Unsets (removes) a variable with `unset <name>`.
  
- **Usage**:
  - Set a variable: `set myvar=hello` (creates a variable `myvar` with the value `hello`).
  - Access a variable: `myvar` (prints `hello`).
  - List all variables: `printenv`.
  - Unset a variable: `unset myvar` (removes `myvar`).

## How to Run the Project
1. **Compile the Code**:
   - Open a terminal in the directory containing your code file (e.g., `shell.c`).
   - Compile the shell with:
     ```bash
     gcc -o myshell shell.c
     ```

2. **Run the Shell**:
   - Start the shell by running:
     ```bash
     ./myshell
     ```

## Testing Each Version

### Version 01: Basic Shell
1. Run commands such as `ls`, `pwd`, and `whoami` to confirm basic execution.
2. Type `exit` or press `<CTRL+D>` to exit the shell.

### Version 02: I/O Redirection
1. Output redirection: `ls > output.txt`. Check `output.txt` for output.
2. Input redirection: Create `unsorted.txt`, then run `sort < unsorted.txt`.
3. Combined redirection: `sort < unsorted.txt > sorted.txt`. Check `sorted.txt`.

### Version 03: Background Processes
1. Start a background job: `sleep 30 &`.
2. View active jobs with `jobs`.
3. Kill a job using `kill <pid>`, where `<pid>` is from `jobs`.

### Version 04: Command History
1. Run several commands (`ls`, `pwd`, `whoami`).
2. Type `history` to view the last 10 commands.
3. Use `!number` (e.g., `!1`) to repeat a command from history.

### Version 05: Built-In Commands
1. `cd /tmp` to test changing directories.
2. Start a background job (`sleep 30 &`), then check with `jobs`.
3. Terminate a job with `kill <pid>` from `jobs`.
4. Type `help` to view all built-in commands.

### Version 06: Shell Variables
1. Set a variable: `set myvar=hello`.
2. Access it by typing `myvar` (should output `hello`).
3. List all variables with `printenv`.
4. Unset `myvar` with `unset myvar` and verify it no longer displays.

## Example Usage
Here’s an example session showing the use of various features across all versions:

```bash
PUCITshell@/home/user:- ls
file1.txt  file2.txt  shell.c
PUCITshell@/home/user:- set myvar=hello
PUCITshell@/home/user:- myvar
hello
PUCITshell@/home/user:- pwd > output.txt
PUCITshell@/home/user:- sleep 30 &
[Background] Started process with PID 1234
PUCITshell@/home/user:- jobs
[1] PID: 1234
PUCITshell@/home/user:- kill 1234
Process 1234 terminated.
PUCITshell@/home/user:- history
1: ls
2: set myvar=hello
3: myvar
4: pwd > output.txt
5: sleep 30 &
6: jobs
PUCITshell@/home/user:- !1
file1.txt  file2.txt  shell.c
PUCITshell@/home/user:- help
Built-in Commands:
cd <directory> - Change directory
exit - Exit the shell
jobs - List background jobs
kill <pid> - Terminate background process with PID
history - Display command history
set <name>=<value> - Set a shell variable
unset <name> - Remove a shell variable
printenv - List shell variables
help - List built-in commands
PUCITshell@/home/user:- printenv
myvar=hello
PUCITshell@/home/user:- unset myvar
PUCITshell@/home/user:- myvar
Error executing command: No such file or directory
PUCITshell@/home/user:- exit
```

## Known Limitations
- **Limited History**: Only stores the last 10 commands.
- **Limited Variables**: Supports a maximum of 50 shell variables.
- **Background Process Limit**: Only allows 10 concurrent background processes.
  
## Summary
This project emulates core functionalities of a UNIX shell, building progressively with each version:
- **Basic command execution**.
- **I/O redirection**.
- **Background job control**.
- **Command history management**.
- **Built-in commands**.
- **Shell variable management**.

Through this project, users gain a deep understanding of process control, input/output management, and command handling in a UNIX-like environment, which are crucial skills for systems programming and operating systems development.

## Acknowledgments
Thanks to the Operating Systems Lab course for providing foundational concepts and guidance, and to online resources on `fork()`, `execvp()`, `dup2()`, and signal handling for aiding the development of this shell.

---
