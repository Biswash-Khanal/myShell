#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "ASTNode.h"






int execute_ast_tree(ASTNode* node, int in_fd, int out_fd);


int execute_command(ASTNode* node, int in_fd, int out_fd);
int execute_redirection(ASTNode* node, int in_fd, int out_fd);
int execute_pipe(ASTNode* node, int in_fd, int out_fd);
int execute_background(ASTNode* node, int in_fd, int out_fd);


void reap_background_zombies();
#endif