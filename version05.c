#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <limits.h>

#define BUFFER_SIZE 1024
pid_t bg_processes[BUFFER_SIZE];  // Array to store background process IDs
int bg_count = 0;  // Count of background processes

void display_prompt() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("PUCITshell@%s:- ", cwd);
    } else {
        perror("getcwd() error");
    }
}

// Function to handle built-in commands
int execute_builtin(char** args) {
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "Expected argument for \"cd\"\n");
        } else if (chdir(args[1]) != 0) {
            perror("cd failed");
        }
        return 1;
    } else if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(args[0], "jobs") == 0) {
        printf("Background Jobs:\n");
        for (int i = 0; i < bg_count; i++) {
            printf("[%d] PID: %d\n", i + 1, bg_processes[i]);
        }
        return 1;
    } else if (strcmp(args[0], "kill") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "Expected PID for \"kill\"\n");
        } else {
            int pid = atoi(args[1]);
            if (kill(pid, SIGKILL) == 0) {
                printf("Process %d terminated.\n", pid);
            } else {
                perror("kill failed");
            }
        }
        return 1;
    } else if (strcmp(args[0], "help") == 0) {
        printf("Built-in Commands:\n");
        printf("cd <directory> - Change directory\n");
        printf("exit - Exit the shell\n");
        printf("jobs - List background jobs\n");
        printf("kill <pid> - Terminate background process with PID\n");
        printf("help - List built-in commands\n");
        return 1;
    }
    return 0;  // Not a built-in command
}

char* read_input() {
    char *buffer = NULL;
    size_t bufsize = 0;
    getline(&buffer, &bufsize, stdin);
    return buffer;
}

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

int execute_command(char** args) {
    pid_t pid = fork();

    if (pid == 0) {  // Child process
        if (execvp(args[0], args) == -1) {
            perror("Error executing command");
        }
        exit(EXIT_FAILURE);

    } else if (pid < 0) {  // Fork error
        perror("Error forking");

    } else {  // Parent process
        waitpid(pid, NULL, 0);  // Wait for foreground process to complete
    }

    return 1;
}

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
