#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "BuiltIns.h"


/**
 * @brief Implementation of the "cd" built-in command.
 *
 * Logic:
 * - If no argument is provided (args[1] == NULL), print an error since "cd" requires a target directory.
 * - Otherwise, attempt to change the working directory using chdir().
 * - If chdir fails (invalid path, permission denied, etc.), log the error with perror.
 * - Return 0 on success, 1 on failure.
 */
int shell_cd(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "myShell: expected argument to \"cd\"\n");
        return 1;
    }

    //GUARD: System call failed
    if (chdir(args[1]) != 0) {
        perror("myShell: cd");
        return 1;
    }

    return 0;
}

/**
 * @brief Implementation of the "pwd" built-in command.
 *
 * Logic:
 * - Use getcwd() to fetch the current working directory.
 * - If getcwd fails, log the error and return failure.
 * - Otherwise, print the directory path to stdout.
 * - Free the allocated buffer returned by getcwd.
 * - Return 0 on success, 1 on failure.
 */
int shell_pwd(char** args) {
    char* cwd = getcwd(NULL, 0);

    if (cwd == NULL) {
        perror("myShell: pwd");
        return 1;
    }

    printf("%s\n", cwd);
    free(cwd);
    return 0;
}

/**
 * @brief Implementation of the "cat" built-in command.
 *
 * Logic:
 * - If no arguments are provided, default to reading from stdin ("-").
 * - Iterate through each argument (file name or "-").
 * - If argument is "-", use STDIN_FILENO as the source.
 * - Otherwise, attempt to open the file for reading.
 *   - On failure, print an error and skip to the next file.
 * - For each source, repeatedly read into a buffer and write to stdout.
 *   - Handle write errors explicitly.
 *   - Handle read errors explicitly.
 * - Close the file descriptor if it was opened (not stdin).
 * - Return 0 on success, 1 if any error occurred.
 */
int shell_cat(char** args) {
    char Buffer[4096];
    ssize_t bytes_read;
    ssize_t bytes_written;

    //Default to stdin if no file arguments are given
    char* default_args[] = { args[0], "-", NULL };
    char** src_args = (args[1] == NULL) ? default_args : args;

    for (int i = 1; src_args[i] != NULL; i++) {
        int fd;
        char* current_target = src_args[i];

        //Special case: "-" means stdin
        if (strcmp(current_target, "-") == 0) {
            fd = STDIN_FILENO;
        }
        else {
            fd = open(current_target, O_RDONLY);
            if (fd < 0) {
                fprintf(stderr, "myShell: cat: %s: %s\n", current_target, strerror(errno));
                continue;
            }
        }

        //Read from source and write to stdout until EOF
        while ((bytes_read = read(fd, Buffer, sizeof(Buffer))) > 0) {
            bytes_written = write(STDOUT_FILENO, Buffer, bytes_read);
            if (bytes_written < 0) {
                fprintf(stderr, "myShell: cat: write error: %s\n", strerror(errno));
                if (fd != STDIN_FILENO) close(fd);
                return 1;
            }
        }

        //Handle read error
        if (bytes_read < 0) {
            fprintf(stderr, "myShell: cat: read error on %s: %s\n", current_target, strerror(errno));
            if (fd != STDIN_FILENO) close(fd);
            return 1;
        }

        //Close file if not stdin
        if (fd != STDIN_FILENO) {
            close(fd);
        }
    }

    return 0;
}

/**
 * @brief Registry table of built-in commands.
 *
 * Each entry maps a command name string to its handler function.
 * This allows the executor to loop through and dispatch built-ins
 * without hardcoding logic for each one.
 */
const BuiltInRegistry builtin_table[] = {
   { "cd",    shell_cd },
   { "pwd",   shell_pwd },
   { "catt",  shell_cat } //NOTE: Typo? "catt" instead of "cat"
};

/**
 * @brief Number of entries in the built-in registry.
 *
 * Computed automatically using sizeof so that adding/removing
 * built-ins does not require manual updates.
 */
const size_t builtin_count = sizeof(builtin_table) / sizeof(builtin_table[0]);
