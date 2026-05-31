#include <stdlib.h>
#include <stdio.h>


#include "ASTNode.h"


ASTNode* create_ast_node(ASTNodeType nodeType) {

    ASTNode* newNode = malloc(sizeof(*newNode));
    // if the pointer returned by malloc is null, memory was not allocated appropriately. return to signal error, or exit the program
    if (newNode == NULL) {
        return NULL;
    }

    //set the type to whatever was passed
    newNode->type = nodeType;

    //initialize as 0 or null for everything else
    newNode->arg_count = 0;

    newNode->right = NULL;
    newNode->left = NULL;


    newNode->file_path = NULL;
    newNode->child_cmd = NULL;


    newNode->arg_values = NULL;

    return newNode;
}

int delete_ast_node(ASTNode* node) {
    //if node is null already, assume its already been freed and return safe
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

    //free the file path, we can just call recursively to delete the child cmd node
    free(node->file_path);
    node->file_path = NULL;

    //free the child cmd node and null the pointer
    delete_ast_node(node->child_cmd);
    node->child_cmd = NULL;

    //recursively free the left and right node and then null the pointer
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

    // 1. Print indentation matching the current depth hierarchy
    for (int i = 0; i < depth; i++) {
        printf("    "); // 4 spaces per depth layer
    }

    // 2. Identify and display the node data based on its structural type
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

    // 3. Recursively print structural dependencies further down the tree
    // If a node has an internal child command payload (like a subshell or specific wrapper)
    if (node->child_cmd != NULL) {
        print_ast_tree(node->child_cmd, depth + 1);
    }

    // Step down left and right child branches
    print_ast_tree(node->left, depth + 1);
    print_ast_tree(node->right, depth + 1);
}