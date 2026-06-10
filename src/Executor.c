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


void reap_background_zombies() {
    int status;
    pid_t died_pid;
    while ((died_pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("\n[Background Process %d completed]\n", died_pid);
    }
}

int exec_builtin(char** args) {
    for (size_t i = 0; i < builtin_count; i++) {
        if (strcmp(args[0], builtin_table[i].name) == 0) {
            int status = builtin_table[i].handler(args);
            exit(status);
        }
    }
    return -1;
}


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


int execute_command(ASTNode* node, int in_fd, int out_fd) {
    if (node == NULL || node->arg_values == NULL || node->arg_values[0] == NULL) {
        return 0;
    }

    char* cmd_name = node->arg_values[0];

    if (strcmp(cmd_name, "cd") == 0 || strcmp(cmd_name, "exit") == 0) {
        for (size_t i = 0; i < builtin_count; i++) {
            if (strcmp(cmd_name, builtin_table[i].name) == 0) {
                return builtin_table[i].handler(node->arg_values);
            }
        }
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("myShell: fork failed");
        return 1;
    }
    else if (pid == 0) {

        if (in_fd != STDIN_FILENO) {
            if (dup2(in_fd, STDIN_FILENO) < 0) {
                perror("myShell: dup2 stdin failed");
                exit(1);
            }
            close(in_fd);
        }

        if (out_fd != STDOUT_FILENO) {
            if (dup2(out_fd, STDOUT_FILENO) < 0) {
                perror("myShell: dup2 stdout failed");
                exit(1);
            }
            close(out_fd);
        }

        exec_builtin(node->arg_values);

        if (execvp(cmd_name, node->arg_values) == -1) {
            fprintf(stderr, "myShell: command not found: %s\n", cmd_name);
            exit(127);
        }
    }
    else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        else if (WIFSIGNALED(status)) {
            return 128 + WTERMSIG(status);
        }
    }

    return 0;
}


int execute_redirection(ASTNode* node, int in_fd, int out_fd) {
    if (node == NULL) return 0;

    int fd = -1;

    if (node->type == NODE_REDIRECT_OUT) {
        fd = open(node->file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            fprintf(stderr, "myShell: %s: %s\n", node->file_path, strerror(errno));
            return 1;
        }
        out_fd = fd;
    }
    else if (node->type == NODE_REDIRECT_APP) {
        fd = open(node->file_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd < 0) {
            fprintf(stderr, "myShell: %s: %s\n", node->file_path, strerror(errno));
            return 1;
        }
        out_fd = fd;
    }
    else if (node->type == NODE_REDIRECT_IN) {
        fd = open(node->file_path, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "myShell: %s: %s\n", node->file_path, strerror(errno));
            return 1;
        }
        in_fd = fd;
    }

    int resultStatus = execute_ast_tree(node->left, in_fd, out_fd);

    if (fd >= 0) {
        close(fd);
    }

    return resultStatus;
}


int execute_pipe(ASTNode* node, int in_fd, int out_fd) {
    if (node == NULL) return 0;

    int pipeEnds[2];
    if (pipe(pipeEnds) != 0) {
        perror("myShell: pipe creation failed");
        return 1;
    }

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("myShell: pipe left fork failed");
        close(pipeEnds[0]); close(pipeEnds[1]);
        return 1;
    }

    if (pid1 == 0) {
        close(pipeEnds[0]);
        // Call unified function recursively
        execute_ast_tree(node->left, in_fd, pipeEnds[1]);
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("myShell: pipe right fork failed");
        close(pipeEnds[0]); close(pipeEnds[1]);
        return 1;
    }

    if (pid2 == 0) {
        close(pipeEnds[1]);
        // Call unified function recursively
        execute_ast_tree(node->right, pipeEnds[0], out_fd);
        exit(0);
    }

    close(pipeEnds[0]);
    close(pipeEnds[1]);

    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

    return WIFEXITED(status2) ? WEXITSTATUS(status2) : 0;
}


int execute_background(ASTNode* node, int in_fd, int out_fd) {
    // 1. Snapshot independent copies right here in the parent frame 
    // if they aren't the standard system defaults.
    int bg_in = in_fd;
    int bg_out = out_fd;

    if (in_fd != STDIN_FILENO) {
        bg_in = dup(in_fd);
    }
    if (out_fd != STDOUT_FILENO) {
        bg_out = dup(out_fd);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("myShell: background fork failed");
        if (bg_in != in_fd) close(bg_in);
        if (bg_out != out_fd) close(bg_out);
        return 1;
    }
    else if (pid == 0) {
 
        int status = execute_ast_tree(node->left, bg_in, bg_out);

        // Clean up our local copies right before dying
        if (bg_in != STDIN_FILENO) close(bg_in);
        if (bg_out != STDOUT_FILENO) close(bg_out);

        exit(status);
    }
    else {

        if (bg_in != in_fd) close(bg_in);
        if (bg_out != out_fd) close(bg_out);

        printf("[1] %d\n", pid);
        return 0;
    }
}