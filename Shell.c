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
    if (getcwd(cwd, sizeof(cwd)) != NULL) {  // Get the current working directory
        printf("PUCITshell@%s:- ", cwd);  // Display the prompt
    } else {
        perror("getcwd() error");  // Handle errors
    }
}

// Function to read the user input from the shell prompt
char* read_input() {
    char *buffer = NULL;  // Initialize buffer for storing input
    size_t bufsize = 0;  // Initial buffer size (getline will allocate memory)
    getline(&buffer, &bufsize, stdin);  // Read input from the user
    return buffer;  // Return the input
}


int main(){
    // This is just initial code I am working on it
    return 0;
}