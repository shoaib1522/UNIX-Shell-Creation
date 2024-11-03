#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>

#define BUFFER_SIZE 1024
#define HISTORY_SIZE 10  // Maximum number of commands in history

char* history[HISTORY_SIZE];  // Array to store command history
int history_count = 0;        // Current count of commands in history

void display_prompt() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("PUCITshell@%s:- ", cwd);
    } else {
        perror("getcwd() error");
    }
}

// Function to add a command to history
void add_to_history(char* input) {
    if (history_count < HISTORY_SIZE) {
        history[history_count++] = strdup(input);
    } else {
        free(history[0]);
        for (int i = 1; i < HISTORY_SIZE; i++) {
            history[i - 1] = history[i];
        }
        history[HISTORY_SIZE - 1] = strdup(input);
    }
}

// Function to display command history
void display_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d: %s", i + 1, history[i]);
    }
}

// Function to retrieve command from history
char* get_command_from_history(int index) {
    if (index < 1 || index > history_count) {
        printf("No such command in history.\n");
        return NULL;
    }
    return strdup(history[index - 1]);
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

        // Handle !number for history
        if (input[0] == '!') {
            int index = atoi(input + 1);
            free(input);
            input = get_command_from_history(index);
            if (input == NULL) {
                continue;
            }
            printf("%s", input);  // Print the command being executed
        } else {
            add_to_history(input);  // Add command to history if not a history command
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
