#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>

#define BUFFER_SIZE 1024  // Buffer size for input

// Function to display the shell prompt with the current working directory
void display_prompt() {
    char cwd[PATH_MAX];  // Buffer to hold the current working directory
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("PUCITshell@%s:- ", cwd);  // Display the shell prompt with directory
    } else {
        perror("getcwd() error");
    }
}

// Function to read the user input
char* read_input() {
    char *buffer = malloc(BUFFER_SIZE * sizeof(char));  // Simple fixed buffer allocation
    if (!buffer) {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }
    
    // For now, let's assume we only handle commands that are a single line long
    fgets(buffer, BUFFER_SIZE, stdin);  // Simple input reading (does not handle errors)
    return buffer;
}

// Very simple function to execute commands, without parsing multiple arguments
int execute_command(char* command) {
    pid_t pid;
    int status;
    
    pid = fork();  // Create a child process
    if (pid == 0) {  // Child process
        // Here we're only executing the command without arguments for now
        if (execlp(command, command, NULL) == -1) {  // Simpler version of exec with no args
            perror("Error executing command");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {  // Fork error
        perror("Error forking");
    } else {
        // Parent process waits for the child process to finish
        waitpid(pid, &status, WUNTRACED);
    }

    return 1;  // Keep the shell running
}

int main() {
    char* input;  // For holding user input
    int status = 1;  // Status to control the loop (1 means keep running)

    do {
        display_prompt();  // Display the prompt
        input = read_input();  // Read input from the user
        
        // Just a simple check to see if the user typed 'exit'
        if (strncmp(input, "exit", 4) == 0) {
            printf("Exiting shell...\n");
            free(input);  // Free the input buffer before exiting
            break;
        }
        
        execute_command(input);

        free(input);  // Free the buffer after each command
    } while (status);  // Keep the shell running

    return 0;
}
