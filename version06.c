#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024
#define MAX_VARS 50  // Maximum number of shell variables

struct var {
    char *name;
    char *value;
    bool global;
};

struct var shell_vars[MAX_VARS];  // Array to store shell variables
int var_count = 0;  // Count of defined shell variables

void display_prompt() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("PUCITshell@%s:- ", cwd);
    } else {
        perror("getcwd() error");
    }
}

// Find the value of a shell variable by name
char* get_variable_value(char* name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(shell_vars[i].name, name) == 0) {
            return shell_vars[i].value;
        }
    }
    return NULL;  // Variable not found
}

// Function to handle built-in commands, including shell variables
int execute_builtin(char** args) {
    if (strcmp(args[0], "set") == 0) {  // Set a shell variable
        char *var_str = args[1];
        if (!var_str) {
            fprintf(stderr, "Usage: set <name>=<value>\n");
            return 1;
        }
        char *eq_pos = strchr(var_str, '=');
        if (!eq_pos) {
            fprintf(stderr, "Usage: set <name>=<value>\n");
            return 1;
        }
        *eq_pos = '\0';  // Split name and value at '='
        char *name = var_str;
        char *value = eq_pos + 1;

        // Update if variable already exists
        for (int i = 0; i < var_count; i++) {
            if (strcmp(shell_vars[i].name, name) == 0) {
                free(shell_vars[i].value);
                shell_vars[i].value = strdup(value);
                return 1;
            }
        }

        // Add new variable
        if (var_count < MAX_VARS) {
            shell_vars[var_count].name = strdup(name);
            shell_vars[var_count].value = strdup(value);
            shell_vars[var_count].global = false;
            var_count++;
        } else {
            fprintf(stderr, "Max number of variables reached.\n");
        }
        return 1;

    } else if (strcmp(args[0], "printenv") == 0) {  // Print all shell variables
        for (int i = 0; i < var_count; i++) {
            printf("%s=%s\n", shell_vars[i].name, shell_vars[i].value);
        }
        return 1;
    } else if (strcmp(args[0], "unset") == 0) {  // Unset a shell variable
        if (!args[1]) {
            fprintf(stderr, "Usage: unset <name>\n");
            return 1;
        }
        for (int i = 0; i < var_count; i++) {
            if (strcmp(shell_vars[i].name, args[1]) == 0) {
                free(shell_vars[i].name);
                free(shell_vars[i].value);
                shell_vars[i] = shell_vars[var_count - 1];  // Remove variable
                var_count--;
                return 1;
            }
        }
        fprintf(stderr, "Variable '%s' not found.\n", args[1]);
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
