#include <stdlib.h>
#include <stdio.h>


#include "ASTNode.h"


ASTNode* create_ast_node(ASTNodeType nodeType) {

    ASTNode* newNode = malloc(sizeof(*newNode));    if (newNode == NULL) {
        return NULL;
    }

    newNode->type = nodeType;

    newNode->arg_count = 0;

    newNode->right = NULL;
    newNode->left = NULL;


    newNode->file_path = NULL;
    newNode->child_cmd = NULL;


    newNode->arg_values = NULL;

    return newNode;
}

int delete_ast_node(ASTNode* node) {
    if (node == NULL) {
        return 1;
    }


    if (node->arg_values != NULL) {
        for (int i = 0; i < node->arg_count; i++) {
            free(node->arg_values[i]);
            node->arg_values[i] = NULL;
        }
        free(node->arg_values);
        node->arg_values = NULL;
    }
    node->arg_count = 0;

    free(node->file_path);
    node->file_path = NULL;

    delete_ast_node(node->child_cmd);
    node->child_cmd = NULL;

    delete_ast_node(node->left);
    node->left = NULL;
    delete_ast_node(node->right);
    node->right = NULL;

    free(node);
    return 1;
}



void print_ast_tree(const ASTNode* node, int depth) {
    if (node == NULL) {
        return;
    }

    for (int i = 0; i < depth; i++) {
        printf("    "); 
    }

    
    switch (node->type) {
    case NODE_COMMAND:
        printf("[COMMAND] Args: ");
        if (node->arg_values != NULL) {
            for (int i = 0; i < node->arg_count; i++) {
                printf("'%s' ", node->arg_values[i]);
            }
        }
        else {
            printf("(none)");
        }
        printf("\n");
        break;

    case NODE_PIPE:
        printf("[PIPE |]\n");
        break;

    case NODE_BACKGROUND:
        printf("[BACKGROUND &]\n");
        break;

    case NODE_REDIRECT_IN:
        printf("[REDIRECT IN <] Target: %s\n", node->file_path ? node->file_path : "NULL");
        break;

    case NODE_REDIRECT_OUT:
        printf("[REDIRECT OUT >] Target: %s\n", node->file_path ? node->file_path : "NULL");
        break;

    case NODE_REDIRECT_APP:
        printf("[REDIRECT APPEND >>] Target: %s\n", node->file_path ? node->file_path : "NULL");
        break;

    default:
        printf("[UNKNOWN NODE TYPE: %d]\n", node->type);
        break;
    }



    if (node->child_cmd != NULL) {
        print_ast_tree(node->child_cmd, depth + 1);
    }


    print_ast_tree(node->left, depth + 1);
    print_ast_tree(node->right, depth + 1);
}