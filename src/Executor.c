#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>   
#include <stdbool.h>

#include "Executor.h"
#include "ASTNode.h"
#include "BuiltIns.h"


/**
 * @brief Utility function to reap background processes that have finished execution.
 *
 * This function uses waitpid with WNOHANG to non-blockingly check for any child processes
 * that have terminated in the background. If found, it prints a message indicating completion.
 */
void reap_background_zombies() {
    int status;
    pid_t died_pid;
    while ((died_pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("\n[Background Process %d completed]\n", died_pid);
    }
}

/**
 * @brief Helper function that checks the built-in registry for the command name
 * and executes it if found.
 *
 * @param args The argument vector containing the command name and its arguments.
 * @return int Returns -1 if not found, otherwise exits with the status returned by the handler.
 */
static int exec_builtin(char** args) {
    for (size_t i = 0; i < builtin_count; i++) {
        if (strcmp(args[0], builtin_table[i].name) == 0) {
            int status = builtin_table[i].handler(args);
            exit(status); // terminate child immediately with handler’s return code
        }
    }
    return -1;
}

/**
 * @brief The utility function for executing a command node.
 *
 * @param node The node to be executed.
 * @param in_fd The input File descriptor.
 * @param out_fd The output File descriptor.
 * @return int Returns status
 */
static int execute_command(ASTNode* node, int in_fd, int out_fd) {
    //If the node itself is NULL, the arg_values is pointing to null or the first argument i.e the command is null, return 
    if (node == NULL || node->arg_values == NULL || node->arg_values[0] == NULL) {
        return 0;
    }

    //store the first argument i.e the command name to a seperate variable
    char* cmd_name = node->arg_values[0];

    //SPECIAL CASES: for commands cd and exit, we dont need to fork into a child process, we can simply run them.
    //Looping throigh the registry for the exceptions might not be necessary but a good fallback incase they dont exist
    if (strcmp(cmd_name, "cd") == 0 || strcmp(cmd_name, "exit") == 0) {
        for (size_t i = 0; i < builtin_count; i++) {
            if (strcmp(cmd_name, builtin_table[i].name) == 0) {
                return builtin_table[i].handler(node->arg_values);
            }
        }
    }

    //Create a child process
    pid_t pid = fork();

    //simple check if fork was succesful
    if (pid < 0) {
        perror("myShell: fork failed");
        return 1;
    }
    //Once confirmed that the fork was successful we branch into 2 process logic
    //pid == 0 for the child process
    //not pid == 0 for the parent process
    else if (pid == 0) {
        //CHILD PROCESS LOGIC

        //If in_fd was passed as STDIN_FILENO, no need to do anything, just close the file descriptor, but if it was not
        //then we use dup2 to now make the STDIN_FILENO point to the in_fd descriptor
        if (in_fd != STDIN_FILENO) {
            //error check and logging for dup2 system call
            if (dup2(in_fd, STDIN_FILENO) < 0) {
                perror("myShell: dup2 stdin failed");
                exit(1);
            }
            close(in_fd);
        }

        //If out_fd was passed as STDOUT_FILENO, no need to do anything, just close the file descriptor, but if it was not
        //then we use dup2 to now make the STDOUT_FILENO point to the out_fd descriptor
        if (out_fd != STDOUT_FILENO) {
            //error check and logging for dup2 system call
            if (dup2(out_fd, STDOUT_FILENO) < 0) {
                perror("myShell: dup2 stdout failed");
                exit(1);
            }
            close(out_fd);
        }

        //Try to execute by searching for command name match on the built in registry. If found, the built in function executes and the child process exits normally.
        exec_builtin(node->arg_values);

        //If not found in the built in registry, we now use the execvp system call to search for the program in the system path, by passing the array of string pointers i.e the arg_values
        //simple error check for the system call, used fprintf with stderr instead of perror to more specialize the error message on which command was not found
        if (execvp(cmd_name, node->arg_values) == -1) {
            fprintf(stderr, "myShell: command not found: %s\n", cmd_name);
            exit(127);
        }
    }
    else {
        //PARENT PROCESS LOGIC:

        //variable to store the child process return code
        int status;

        //use waitpid system call with option 0, to wait for the child process to termintate, and store the status code
        waitpid(pid, &status, 0);

        //now we do different conditional checks on the status 
        if (WIFEXITED(status)) {
            // Child ended normally (return from main or exit())
            return WEXITSTATUS(status);
        }
        else if (WIFSIGNALED(status)) {
            // Child was killed by a signal
            return 128 + WTERMSIG(status);
        }
        else if (WIFSTOPPED(status)) {
            // Child was stopped (Ctrl-Z, SIGSTOP)
            // Shells usually don’t return here, they manage jobs
            fprintf(stderr, "Process stopped by signal %d\n", WSTOPSIG(status));
        }
        else if (WIFCONTINUED(status)) {
            // Child resumed after being stopped
            fprintf(stderr, "Process continued\n");
        }
    }

    return 0;
}

/**
 * @brief Executes a redirection node by opening the target file and
 * redirecting input/output accordingly, then recursively executing the left subtree.
 *
 * @param node The redirection AST node.
 * @param in_fd Current input file descriptor.
 * @param out_fd Current output file descriptor.
 * @return int Status code of the executed subtree.
 */
static int execute_redirection(ASTNode* node, int in_fd, int out_fd) {
    if (node == NULL) return 0;

    int fd = -1;

    //Handle output redirection (overwrite)
    if (node->type == NODE_REDIRECT_OUT) {
        fd = open(node->file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            fprintf(stderr, "myShell: %s: %s\n", node->file_path, strerror(errno));
            return 1;
        }
        out_fd = fd;
    }
    //Handle output redirection (append)
    else if (node->type == NODE_REDIRECT_APP) {
        fd = open(node->file_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd < 0) {
            fprintf(stderr, "myShell: %s: %s\n", node->file_path, strerror(errno));
            return 1;
        }
        out_fd = fd;
    }
    //Handle input redirection
    else if (node->type == NODE_REDIRECT_IN) {
        fd = open(node->file_path, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "myShell: %s: %s\n", node->file_path, strerror(errno));
            return 1;
        }
        in_fd = fd;
    }

    //Recursively execute the left subtree with updated file descriptors
    int resultStatus = execute_ast_tree(node->left, in_fd, out_fd);

    //Close the file descriptor if it was opened
    if (fd >= 0) {
        close(fd);
    }

    return resultStatus;
}

/**
 * @brief Executes a pipe node by creating a pipe, forking two child processes,
 * and connecting their input/output streams.
 *
 * @param node The pipe AST node.
 * @param in_fd Current input file descriptor.
 * @param out_fd Current output file descriptor.
 * @return int Status code of the right-hand command.
 */
static int execute_pipe(ASTNode* node, int in_fd, int out_fd) {
    if (node == NULL) return 0;

    //Create a pipe with two ends: pipeEnds[0] for reading, pipeEnds[1] for writing
    int pipeEnds[2];
    if (pipe(pipeEnds) != 0) {
        perror("myShell: pipe creation failed");
        return 1;
    }

    //Fork the left side of the pipe
    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("myShell: pipe left fork failed");
        close(pipeEnds[0]); close(pipeEnds[1]);
        return 1;
    }

    if (pid1 == 0) {
        //CHILD PROCESS for left command
        close(pipeEnds[0]); //close unused read end
        //Execute left subtree with output redirected to pipe write end
        execute_ast_tree(node->left, in_fd, pipeEnds[1]);
        exit(0);
    }

    //Fork the right side of the pipe
    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("myShell: pipe right fork failed");
        close(pipeEnds[0]); close(pipeEnds[1]);
        return 1;
    }

    if (pid2 == 0) {
        //CHILD PROCESS for right command
        close(pipeEnds[1]); //close unused write end
        //Execute right subtree with input redirected to pipe read end
        execute_ast_tree(node->right, pipeEnds[0], out_fd);
        exit(0);
    }

    //PARENT PROCESS: close both ends of the pipe, since children handle them
    close(pipeEnds[0]);
    close(pipeEnds[1]);

    //Wait for both children to finish
    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

    //Return the exit status of the right-hand command (common shell convention)
    return WIFEXITED(status2) ? WEXITSTATUS(status2) : 0;
}

/**
 * @brief Executes a background node by forking a child process that runs independently.
 *
 * @param node The background AST node.
 * @param in_fd Current input file descriptor.
 * @param out_fd Current output file descriptor.
 * @return int Always returns 0 to the parent, since background jobs run asynchronously.
 */
static int execute_background(ASTNode* node, int in_fd, int out_fd) {
    //Snapshot independent copies of file descriptors if they aren’t the defaults
    int bg_in = in_fd;
    int bg_out = out_fd;

    if (in_fd != STDIN_FILENO) {
        bg_in = dup(in_fd);
    }
    if (out_fd != STDOUT_FILENO) {
        bg_out = dup(out_fd);
    }

    //Fork a child process to run the background job
    pid_t pid = fork();

    if (pid < 0) {
        perror("myShell: background fork failed");
        if (bg_in != in_fd) close(bg_in);
        if (bg_out != out_fd) close(bg_out);
        return 1;
    }
    else if (pid == 0) {
        //CHILD PROCESS: execute the left subtree in background
        int status = execute_ast_tree(node->left, bg_in, bg_out);

        //Clean up local copies before exiting
        if (bg_in != STDIN_FILENO) close(bg_in);
        if (bg_out != STDOUT_FILENO) close(bg_out);

        exit(status);
    }
    else {
        //PARENT PROCESS: close duplicated descriptors
        if (bg_in != in_fd) close(bg_in);
        if (bg_out != out_fd) close(bg_out);

        //Print job info (simple job ID and PID)
        printf("[1] %d\n", pid);
        return 0;
    }
}

/**
 * @brief Unified dispatcher function that executes an AST tree node
 * based on its type (command, redirection, pipe, background).
 *
 * @param node The AST node to execute.
 * @param in_fd Current input file descriptor.
 * @param out_fd Current output file descriptor.
 * @return int Status code returned by the executed node.
 */
int execute_ast_tree(ASTNode* node, int in_fd, int out_fd) {
    if (node == NULL) {
        return 0;
    }

    switch (node->type) {
    case NODE_COMMAND:
        return execute_command(node, in_fd, out_fd);
    case NODE_REDIRECT_IN:
    case NODE_REDIRECT_OUT:
    case NODE_REDIRECT_APP:
        return execute_redirection(node, in_fd, out_fd);
    case NODE_PIPE:
        return execute_pipe(node, in_fd, out_fd);
    case NODE_BACKGROUND:
        return execute_background(node, in_fd, out_fd);
    default:
        fprintf(stderr, "myShell Error: Unknown execution node type\n");
        return -1;
    }
}
