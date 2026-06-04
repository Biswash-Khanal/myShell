#include "Executor.h"
#include <stdio.h>
#include <stdlib.h>

// Every custom command you write will follow this exact signature protocol
typedef int (*BuiltInHandler)(char** args);

typedef struct {
    const char* name;
    BuiltInHandler handler;
} BuiltInRegistry;

// Forward declarations of your custom commands
int shell_cd(char** args);
int shell_pwd(char** args);
int shell_cat(char** args);

// THE LOOKUP REGISTRY: To add a command to your shell later, you just add ONE line here.
static const BuiltInRegistry builtin_table[] = {
    { "cd",    shell_cd },
    { "pwd",   shell_pwd },
    { "cat",   shell_cat }
};
static const size_t builtin_count = sizeof(builtin_table) / sizeof(builtin_table[0]);


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

int execute_command(ASTNode* node) {
    printf("\nNormal Command node. Executiong command!");
    printf("\nCommand detected as %s with %d args", node->arg_values[0], node->arg_count);
    return 0;
}
int execute_redirection(ASTNode* node) {
    printf("\nNormal Redirection node. Executiong command!");
    printf("\nRedirection detected as %d with %s filepath", node->type, node->file_path);
    return 0;
}
int execute_pipe(ASTNode* node) {
    printf("\n| operator node detected");
    printf("\nThis will have a left node and a right node thaat needds to be executed sequentially while feeding the output of one as the input of the other");
    return 0;
}
int execute_background(ASTNode* node) {
    printf("\nNormal Command node. Executiong command!");
    printf("\nThe whole command tree under it needs to be ran in the background , probably with threads");
    return 0;
}