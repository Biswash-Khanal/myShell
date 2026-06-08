#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // For fork(), execvp(), chdir(), getcwd(), close()
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>   // For waitpid()

#include "Executor.h"
#include "ASTNode.h"
#include "BuiltIns.h"


int exec_builtin(char** args) {
    for (size_t i = 0; i < builtin_count; i++) {
        if (strcmp(args[0], builtin_table[i].name) == 0) {
            // Execute the local C function inside the child sandbox
            int status = builtin_table[i].handler(args);
            // Vaporize the child process here so it matches execvp's behavior!
            exit(status);
        }
    }
    return -1; // Not a built-in, hand control back to try execvp
}


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

    char* cmd_name = node->arg_values[0];

    // STEP 1: The Parent-Only Exception Backstop
    // "cd" and "exit" CANNOT fork, so we handle them immediately in the parent.
    if (strcmp(cmd_name, "cd") == 0 || strcmp(cmd_name, "exit") == 0) {
        for (size_t i = 0; i < builtin_count; i++) {
            if (strcmp(cmd_name, builtin_table[i].name) == 0) {
                return builtin_table[i].handler(node->arg_values);
            }
        }
    }

    // STEP 2: Unified Forking Path (For all externals AND forkable built-ins like cat)
    pid_t pid = fork();

    if (pid < 0) {
        perror("myShell: fork failed");
        return 1;
    }
    else if (pid == 0) {
        // ---- CHILD PROCESS SANDBOX ----

        // 1. Try running it as a built-in wrapper. 
        // If it matches 'cat', it runs and calls exit() inside. It never reaches line 2!
        exec_builtin(node->arg_values);

        // 2. Fallback: If it wasn't a built-in, exec_builtin returned -1, so we try disk binaries.
        if (execvp(cmd_name, node->arg_values) == -1) {
            fprintf(stderr, "myShell: command not found: %s\n", cmd_name);
            exit(127);
        }
    }
    else {
        // ---- PARENT PROCESS ----
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
    }

    return 0;
}

/**
 * @brief Handles output/input tracking manipulations with multi-layered traversal support.
 */
int execute_redirection(ASTNode* node) {
    if (node == NULL) return 0;

    pid_t pid = fork();

    if (pid < 0) {
        perror("myShell: redirection fork failed");
        return 1;
    }

    if (pid == 0) {
        // ---- CHILD PROCESS SANDBOX ----
        ASTNode* cmd_node = node;

        while (cmd_node->type == NODE_REDIRECT_IN ||
            cmd_node->type == NODE_REDIRECT_OUT ||
            cmd_node->type == NODE_REDIRECT_APP) {

            int fd = -1;
            if (cmd_node->type == NODE_REDIRECT_OUT) {
                fd = open(cmd_node->file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                dup2(fd, STDOUT_FILENO);
            }
            else if (cmd_node->type == NODE_REDIRECT_APP) {
                fd = open(cmd_node->file_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
                dup2(fd, STDOUT_FILENO);
            }
            else if (cmd_node->type == NODE_REDIRECT_IN) {
                fd = open(cmd_node->file_path, O_RDONLY);
                dup2(fd, STDIN_FILENO);
            }

            if (fd < 0) {
                fprintf(stderr, "myShell: %s: %s\n", cmd_node->file_path, strerror(errno));
                exit(1);
            }
            close(fd);

            // Keep sliding down left branches
            cmd_node = cmd_node->left;
        }

        // Out of the loop! Run the command payload inside the rewired child context
        if (cmd_node != NULL && cmd_node->type == NODE_COMMAND) {
            // Try local built-in wrapper first
            exec_builtin(cmd_node->arg_values);

            // Fallback to external disk binaries
            execvp(cmd_node->arg_values[0], cmd_node->arg_values);

            fprintf(stderr, "myShell: command not found: %s\n", cmd_node->arg_values[0]);
            exit(127);
        }

        // Safety exit path if a tree terminates without a trailing command leaf
        exit(0);
    }
    else {
        // ---- REAL PARENT PROCESS CONTEXT ----
        // This is now properly aligned outside the if (pid == 0) boundary!
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    }
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