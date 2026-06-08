#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "BuiltIns.h"

//Some implementations of simple versions of built in shell commands not complete yet

//Tbe change directory command
int shell_cd(char** args) {
    // GUARD: No argument provided
    if (args[1] == NULL) {
        fprintf(stderr, "myShell: expected argument to \"cd\"\n");
        return 1; // Error code
    }

    // GUARD: System call failed
    if (chdir(args[1]) != 0) {
        perror("myShell: cd");
        return 1; // Error code
    }

    return 0; // Success!
}

int shell_pwd(char** args) {
    // Dynamic approach: getcwd allocates the string for us
    char* cwd = getcwd(NULL, 0);

    // GUARD: Failed to get directory
    if (cwd == NULL) {
        perror("myShell: pwd");
        return 1;
    }

    printf("%s\n", cwd);
    free(cwd); // Clean up the memory getcwd allocated
    return 0; // Success!
}

int shell_cat(char** args) {
    char Buffer[4096];
    ssize_t bytes_read;
    ssize_t bytes_written;

    // Simulate standard input if no file arguments are present
    char* default_args[] = { args[0], "-", NULL };
    char** src_args = (args[1] == NULL) ? default_args : args;

    for (int i = 1; src_args[i] != NULL; i++) {
        int fd;
        char* current_target = src_args[i];

        // 1. Establish the correct Descriptor
        if (strcmp(current_target, "-") == 0) {
            fd = STDIN_FILENO;
        }
        else {
            fd = open(current_target, O_RDONLY);
            if (fd < 0) {
                // Professional target-specific error reporting
                fprintf(stderr, "myShell: cat: %s: %s\n", current_target, strerror(errno));
                continue;
            }
        }

        // 2. Core Processing Stream
        while ((bytes_read = read(fd, Buffer, sizeof(Buffer))) > 0) {
            bytes_written = write(STDOUT_FILENO, Buffer, bytes_read);
            if (bytes_written < 0) {
                fprintf(stderr, "myShell: cat: write error: %s\n", strerror(errno));
                if (fd != STDIN_FILENO) close(fd);
                return 1;
            }
        }

        // FIX: Catch the read failure immediately right here before discarding the fd
        if (bytes_read < 0) {
            fprintf(stderr, "myShell: cat: read error on %s: %s\n", current_target, strerror(errno));
            if (fd != STDIN_FILENO) close(fd);
            return 1;
        }

        // 3. Resource Cleanup safely
        if (fd != STDIN_FILENO) {
            close(fd);
        }
    }

    return 0;
}

// THE LOOKUP REGISTRY: Matches string input to the C function addresses above
const BuiltInRegistry builtin_table[] = {
   { "cd",    shell_cd },
   { "pwd",   shell_pwd },
   { "catt",   shell_cat }
};

//dynamically calculate the number of available built in commands for later looping through to match the name
const size_t builtin_count = sizeof(builtin_table) / sizeof(builtin_table[0]);
