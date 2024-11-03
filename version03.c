#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
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
            bg_processes[bg_count++] = pid;  // Store background process ID
        } else {
            waitpid(pid, NULL, 0);  // Wait for foreground process to complete
        }
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
            status = execute_command(args);
        }

        free(input);
        free(args);
    } while (status);

    return 0;
}
