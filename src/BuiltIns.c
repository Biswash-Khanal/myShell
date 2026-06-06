#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "BuiltIns.h"



//Some implementations of simple versions of built in shell commands not complete yet

//Tbe change directory command
int shell_cd(char** args) {
    //if no other arguments other than the command itself, send error
    if (args[1] == NULL) {
        fprintf(stderr, "myShell: expected argument to \"cd\"\n");
        return 1;
    }

    //we use the chdir system call to change the process' working difrectory, if the chdir returns failure, we return error
    if (chdir(args[1]) != 0) {
        perror("myShell: cd failed");
        return 1;
    }

    //If successful weve changed the working directory

    return 0;
}

//small and simple implementation of pwd, with the help of system call getcwd
int shell_pwd(char** args) {
    char buffer[1024];
    if (getcwd(buffer, sizeof(buffer)) != NULL) {
        printf("%s\n", buffer);
        return 0;
    }
    perror("myShell: pwd failed");
    return 1;
}


int shell_cat(char** args) {
    char buffer[4096]; // A standard 4KB rolling page buffer
    ssize_t bytes_read;
    ssize_t bytes_written;

    // =========================================================================
    // CASE 1: No arguments provided (e.g., just "cat")
    // Read directly from standard input stream (0) until User presses Ctrl+D (EOF)
    // =========================================================================
    if (args[1] == NULL) {
        // read() from file descriptor 0 blocks and waits for terminal input automatically
        while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
            bytes_written = write(STDOUT_FILENO, buffer, bytes_read);
            if (bytes_written < 0) {
                perror("myShell: cat write error");
                return 1;
            }
        }
        if (bytes_read < 0) {
            perror("myShell: cat read error");
            return 1;
        }
        return 0;
    }

    // =========================================================================
    // CASE 2: One or more file arguments provided (e.g., "cat file1.txt file2.txt")
    // Loop through each argument sequentially
    // =========================================================================
    for (int i = 1; args[i] != NULL; i++) {
        // Open the file in Read-Only mode
        int fd = open(args[i], O_RDONLY);
        if (fd < 0) {
            // Localized runtime error tracking if the file doesn't exist or is protected
            fprintf(stderr, "myShell: cat: %s: No such file or directory\n", args[i]);
            continue; // Skip this file and try the next one in the argument array
        }

        // Stream the file contents through our buffer to standard output (1)
        while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
            bytes_written = write(STDOUT_FILENO, buffer, bytes_read);
            if (bytes_written < 0) {
                perror("myShell: cat write error");
                close(fd);
                return 1;
            }
        }

        // Always clean up your file descriptors when finished!
        close(fd);
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
