#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "ASTNode.h"





/**
 *@brief This function is used to execute the parsed AST.
 *
 * @param rootNode The pointer to the root node of the AST
 * @return The exit status code of the executed commands
 */
int execute_ast_tree(ASTNode* node, int in_fd, int out_fd);

//TODO proper documentation of each functions

int execute_command(ASTNode* node, int in_fd, int out_fd);
int execute_redirection(ASTNode* node, int in_fd, int out_fd);
int execute_pipe(ASTNode* node, int in_fd, int out_fd);
int execute_background(ASTNode* node, int in_fd, int out_fd);

/**
 * @brief Asynchronously reaps any background zombie processes.
 * Call this at the top of your main user input loop right before displaying the prompt!
 */
void reap_background_zombies();
#endif