#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "ASTNode.h"

/**
 *@brief This function is used to execute the parsed AST.
 *
 * @param rootNode The pointer to the root node of the AST
 * @return The exit status code of the executed commands
 */
int execute_ast_tree(ASTNode* node);

//TODO proper documentation of each functions

int execute_command(ASTNode* node);
int execute_redirection(ASTNode* node);
int execute_pipe(ASTNode* node);
int execute_background(ASTNode* node);

#endif