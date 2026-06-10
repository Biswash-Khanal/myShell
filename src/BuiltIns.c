#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "BuiltIns.h"




int shell_cd(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "myShell: expected argument to \"cd\"\n");
        return 1; 
    }

    // GUARD: System call failed
    if (chdir(args[1]) != 0) {
        perror("myShell: cd");
        return 1; 
    }

    return 0; 
}

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

int shell_cat(char** args) {
    char Buffer[4096];
    ssize_t bytes_read;
    ssize_t bytes_written;

   
    char* default_args[] = { args[0], "-", NULL };
    char** src_args = (args[1] == NULL) ? default_args : args;

    for (int i = 1; src_args[i] != NULL; i++) {
        int fd;
        char* current_target = src_args[i];

       
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

        
        while ((bytes_read = read(fd, Buffer, sizeof(Buffer))) > 0) {
            bytes_written = write(STDOUT_FILENO, Buffer, bytes_read);
            if (bytes_written < 0) {
                fprintf(stderr, "myShell: cat: write error: %s\n", strerror(errno));
                if (fd != STDIN_FILENO) close(fd);
                return 1;
            }
        }

        
        if (bytes_read < 0) {
            fprintf(stderr, "myShell: cat: read error on %s: %s\n", current_target, strerror(errno));
            if (fd != STDIN_FILENO) close(fd);
            return 1;
        }

        
        if (fd != STDIN_FILENO) {
            close(fd);
        }
    }

    return 0;
}


const BuiltInRegistry builtin_table[] = {
   { "cd",    shell_cd },
   { "pwd",   shell_pwd },
   { "catt",   shell_cat }
};


const size_t builtin_count = sizeof(builtin_table) / sizeof(builtin_table[0]);
