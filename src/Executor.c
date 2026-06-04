#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // For fork(), execvp(), chdir(), getcwd(), close()
#include <sys/wait.h>   // For waitpid()
#include <fcntl.h>      // For open() flags (O_RDONLY, O_WRONLY, etc.)

#include "Executor.h"
#include "ASTNode.h"



/**
 * @brief a typedef for a function pointer type that can point to our command implementation functions
 * These functions will take the command arguments as their arguments, and we use these argument to find, verify signature and execute the command
 * 
 */
typedef int (*BuiltInHandler)(char** args);

/**
 * @brief The structure that binds together the name of the command and the pointer to the function to execute the command
 * 
 */
typedef struct {
    const char* name;
    BuiltInHandler handler;
} BuiltInRegistry;


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


//TODO implementation of cat
int shell_cat(char** args) {
    printf("[MOCK BUILTIN] Executing custom cat logic later for streams!\n");
    return 0;
}

// THE LOOKUP REGISTRY: Matches string input to the C function addresses above
 const BuiltInRegistry builtin_table[] = {
    { "cd",    shell_cd },
    { "pwd",   shell_pwd },
    { "cat",   shell_cat }
};

//dynamically calculate the number of available built in commands for later looping through to match the name
 const size_t builtin_count = sizeof(builtin_table) / sizeof(builtin_table[0]);

/**
 * @brief Master entry point. Traverses the tree based on node metadata. Simple implementation with switch statement. Appropriate execution function is called based on the node type
 */
int execute_ast_tree(ASTNode* node) {
    if (node == NULL) {
        return 0;
    }

    switch (node->type) {
        case NODE_COMMAND:
            return execute_command(node);
        case NODE_REDIRECT_IN:
        case NODE_REDIRECT_OUT:
        case NODE_REDIRECT_APP:
            return execute_redirection(node);
        case NODE_PIPE:
            return execute_pipe(node);
        case NODE_BACKGROUND:
            return execute_background(node);
        default:
            fprintf(stderr, "myShell Error: Unknown execution node type\n");
            return -1;
    }
}

/**
 * @brief Handles terminal expressions. Differentiates built-ins vs external software binaries. 
 */
 int execute_command(ASTNode* node) {
    if (node == NULL || node->arg_values == NULL || node->arg_values[0] == NULL) {
        return 0;
    }

    // STEP 1: Loop through the built in registry, comparing the command to the function name
    for (size_t i = 0; i < builtin_count; i++) {
        if (strcmp(node->arg_values[0], builtin_table[i].name) == 0) {
            // If found a match, we can immediately execute it and return from the function
            return builtin_table[i].handler(node->arg_values);
        }
    }

    // STEP 2: If it's not a built-in, clone the process space to run an external program (e.g. ls, mkdir)

    //create a child process
    pid_t pid = fork();

    //making child process failed if pid is negative
    if (pid < 0) {
        // Severe Kernel Error: Forking failed entirely
        perror("myShell: system fork failed");
        return 1;
    } 
    //The branch that runs on the child process itself
    else if (pid == 0) {
        //exec
        if (execvp(node->arg_values[0], node->arg_values) == -1) {
            // If execvp returns, it means the command name was completely invalid or missing on disk
            fprintf(stderr, "myShell: command not found: %s\n", node->arg_values[0]);
            exit(127); // Exit child cleanly with standard Unix 'command not found' status code
        }
    } 
    //the branch that only runs on the main process itself
    else {
        // PARENT PROCESS CONTEXT: Halt the shell loop line until our child finishes execution
        int status;
        waitpid(pid, &status, 0);
        
        // Return the clean terminating exit status of the child program back up the tree
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
    }

    return 0;
}

/**
 * @brief Handles output/input tracking manipulations.
 */
 int execute_redirection(ASTNode* node) {
    // Structural placeholder tracker to prove recursive routing is flawless
    printf("[DEBUG] Redirection node hit for file: %s. Re-routing tree downward...\n", node->file_path);
    
    // For right now, it simply slides straight down to execute the command underneath it!
    return execute_ast_tree(node->left);
}

/**
 * @brief Handles multi-stage sequential stream processing.
 */
 int execute_pipe(ASTNode* node) {
    printf("[DEBUG] Pipeline operator node hit. Traversing branches...\n");
    
    // For right now, it runs them sequentially so you can see your commands parse out
    execute_ast_tree(node->left);
    execute_ast_tree(node->right);
    return 0;
}

/**
 * @brief Asynchronously isolates child branches.
 */
 int execute_background(ASTNode* node) {
    printf("[DEBUG] Background operational envelope hit.\n");
    return execute_ast_tree(node->left);
} 