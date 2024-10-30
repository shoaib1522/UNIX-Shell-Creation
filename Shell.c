#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <limits.h>

#define BUFFER_SIZE 1024  // Buffer size for input

// Function to display the shell prompt with the current working directory
void display_prompt() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("PUCITshell@%s:- ", cwd);
    } else {
        perror("getcwd() error");
    }
}

// Function to read the user input from the shell prompt
char* read_input() {
    char *buffer = NULL;
    size_t bufsize = 0;
    getline(&buffer, &bufsize, stdin);
    return buffer;
}

// Function to parse input and split by spaces
char** parse_input(char* input) {
    int bufsize = BUFFER_SIZE, position = 0;
    char** tokens = malloc(bufsize * sizeof(char*));
    char* token;

    if (!tokens) {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(input, " \t\r\n\a");
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += BUFFER_SIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "Reallocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, " \t\r\n\a");
    }
    tokens[position] = NULL;
    return tokens;
}

// Function to execute commands with optional I/O redirection and background process handling
int execute_command(char** args) {
    int in_redirect = -1, out_redirect = -1;
    int background = 0;
    int i = 0;

    // Check for background process symbol '&' at the end
    while (args[i] != NULL) {
        if (strcmp(args[i], "&") == 0) {
            background = 1;
            args[i] = NULL;  // Remove '&' from the arguments list
            break;
        }
        i++;
    }

    i = 0;
    // Identify any redirection operators and open the files
    while (args[i] != NULL) {
        if (strcmp(args[i], "<") == 0) {
            in_redirect = open(args[i + 1], O_RDONLY);
            if (in_redirect < 0) {
                perror("Error opening input file");
                return 1;
            }
            args[i] = NULL;  // Remove redirection from args
        } else if (strcmp(args[i], ">") == 0) {
            out_redirect = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_redirect < 0) {
                perror("Error opening output file");
                return 1;
            }
            args[i] = NULL;  // Remove redirection from args
        }
        i++;
    }

    pid_t pid = fork();
    if (pid == 0) {  // Child process
        // Handle input redirection
        if (in_redirect != -1) {
            dup2(in_redirect, STDIN_FILENO);
            close(in_redirect);
        }
        // Handle output redirection
        if (out_redirect != -1) {
            dup2(out_redirect, STDOUT_FILENO);
            close(out_redirect);
        }

        // Execute the command
        if (execvp(args[0], args) == -1) {
            perror("Error executing command");
        }
        exit(EXIT_FAILURE);

    } else if (pid < 0) {  // Fork error
        perror("Error forking");

    } else {  // Parent process
        if (background) {
            // Background process: don't wait, print process ID
            printf("[Background] Started process with PID %d\n", pid);
        } else {
            // Foreground process: wait for child to finish
            waitpid(pid, NULL, 0);
        }
    }

    return 1;  // Keep shell running
}

// Main shell loop
int main() {
    char* input;
    char** args;
    int status;

    do {
        display_prompt();
        input = read_input();

        // Exit on Ctrl+D (EOF)
        if (feof(stdin)) {
            printf("\n");
            break;
        }

        args = parse_input(input);
        if (args[0] != NULL) {
            status = execute_command(args);
        }

        free(input);
        free(args);
    } while (status);

    return 0;
}
