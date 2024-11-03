#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <limits.h>

#define BUFFER_SIZE 1024
#define MAX_BG_PROCESSES 50

pid_t bg_processes[MAX_BG_PROCESSES];  // Array to store background process IDs
int bg_count = 0;  // Count of background processes

// Function to display the shell prompt with the current working directory
void display_prompt() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("PUCITshell@%s:- ", cwd);
    } else {
        perror("getcwd() error");
    }
}

// Function to handle built-in commands, including 'jobs'
int execute_builtin(char** args) {
    if (strcmp(args[0], "jobs") == 0) {
        printf("Background Jobs:\n");
        for (int i = 0; i < bg_count; i++) {
            int status;
            pid_t result = waitpid(bg_processes[i], &status, WNOHANG);
            if (result == 0) {
                printf("[%d] PID: %d\n", i + 1, bg_processes[i]);  // Still running
            } else {
                // If the process is finished, remove it from the list
                for (int j = i; j < bg_count - 1; j++) {
                    bg_processes[j] = bg_processes[j + 1];
                }
                bg_count--;
                i--;  // Re-check the new process in this position
            }
        }
        return 1;
    }
    return 0;  // Not a built-in command
}

// Function to read user input from the shell prompt
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

// Function to execute commands with background process handling
int execute_command(char** args) {
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

    pid_t pid = fork();
    if (pid == 0) {  // Child process
        if (execvp(args[0], args) == -1) {
            perror("Error executing command");
        }
        exit(EXIT_FAILURE);

    } else if (pid < 0) {  // Fork error
        perror("Error forking");

    } else {  // Parent process
        if (background) {
            printf("[Background] Started process with PID %d\n", pid);
            if (bg_count < MAX_BG_PROCESSES) {
                bg_processes[bg_count++] = pid;  // Store background process ID
            }
        } else {
            waitpid(pid, NULL, 0);  // Wait for foreground process to complete
        }
    }

    return 1;
}

// Main shell loop
int main() {
    char* input;
    char** args;
    int status = 1;

    do {
        display_prompt();
        input = read_input();

        if (feof(stdin)) {
            printf("\n");
            break;
        }

        args = parse_input(input);
        if (args[0] != NULL) {
            if (execute_builtin(args) == 0) {  // Run built-in or external command
                status = execute_command(args);
            }
        }

        free(input);
        free(args);
    } while (status);

    return 0;
}
