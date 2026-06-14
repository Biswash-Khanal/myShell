#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "ASTNode.h"

/**
 * @brief The main execution function that takes in the AST tree and runs the commands
 * 
 * @param node The root node of the parsed AST.
 * @param in_fd The input file descriptor. (Set it to STDIN_FILENO if unsure)
 * @param out_fd The output file descriptor. (Set it to STDOUT_FILENO if unsure)
 * @return int return the code 0 for success and other for failure
 */
int execute_ast_tree(ASTNode* node, int in_fd, int out_fd);

void reap_background_zombies();
#endif