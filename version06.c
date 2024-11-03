#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <limits.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024
#define MAX_VARS 50          // Maximum number of shell variables
#define MAX_BG_PROCESSES 10  // Maximum number of background processes
#define HISTORY_SIZE 10      // Maximum number of commands in history

// Structure to store shell variables
struct var {
    char *name;
    char *value;
    bool global;
};

struct var shell_vars[MAX_VARS];  // Array to store shell variables
int var_count = 0;  // Count of defined shell variables
char* history[HISTORY_SIZE];  // Array to store command history
int history_count = 0;        // Current count of commands in history
pid_t bg_processes[MAX_BG_PROCESSES];  // Array to store background process IDs
int bg_count = 0;             // Count of background processes

// Function to display the shell prompt with the current working directory
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

// Function to find a shell variable by name
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
    // Handle 'cd' command
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "Expected argument for \"cd\"\n");
        } else if (chdir(args[1]) != 0) {
            perror("cd failed");
        }
        return 1;
    }

    // Handle 'exit' command
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }

    // Handle 'jobs' command
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
                i--;
            }
        }
        return 1;
    }

    // Handle 'kill' command
    if (strcmp(args[0], "kill") == 0) {
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
    }

    // Handle 'set <name>=<value>' to define a shell variable
    if (strcmp(args[0], "set") == 0) {
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

        // Update variable if it already exists
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
    }

    // Handle 'unset <name>' to remove a shell variable
    if (strcmp(args[0], "unset") == 0) {
        if (!args[1]) {
            fprintf(stderr, "Usage: unset <name>\n");
            return 1;
        }
        for (int i = 0; i < var_count; i++) {
            if (strcmp(shell_vars[i].name, args[1]) == 0) {
                free(shell_vars[i].name);
                free(shell_vars[i].value);
                shell_vars[i] = shell_vars[var_count - 1];
                var_count--;
                return 1;
            }
        }
        fprintf(stderr, "Variable '%s' not found.\n", args[1]);
        return 1;
    }

    // Handle 'printenv' to list all shell variables
    if (strcmp(args[0], "printenv") == 0) {
        for (int i = 0; i < var_count; i++) {
            printf("%s=%s\n", shell_vars[i].name, shell_vars[i].value);
        }
        return 1;
    }

    // Check if the command is a shell variable name
    char* value = get_variable_value(args[0]);
    if (value) {
        printf("%s\n", value);
        return 1;
    }

    return 0;  // Not a built-in command
}

// Function to read user input from the shell prompt
char* read_input() {
    char *buffer = NULL;
    size_t bufsize = 0;
    ssize_t len = getline(&buffer, &bufsize, stdin);

    // Check if the user entered Ctrl+D (EOF)
    if (len == -1) {
        if (feof(stdin)) {  // End of input
            printf("\n");
            free(buffer);
            exit(0);  // Gracefully exit the shell
        }
    }
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
            if (execute_builtin(args) == 0) {  // Run built-in or external command
                status = execute_command(args);
            }
        }

        free(input);
        free(args);
    } while (status);

    return 0;
}
