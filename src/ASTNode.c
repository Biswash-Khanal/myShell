#include <stdlib.h>
#include <stdio.h>


#include "ASTNode.h"
#include "MemoryGuard.h"


ASTNode* create_ast_node(ASTNodeType nodeType) {

    //create a new ASTnode object with malloc
    ASTNode* newNode = malloc(sizeof(*newNode));
    //simple malloc check
    MEMORY_GUARD(newNode, __FILE__, __LINE__);

    //initialize the ASTNode based on the nodeType passed
    //for cetainity, all the fields, even the ones unused for the nodetype are initialized
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
    //if the passed node is already null, assume its already been freed
    if (node == NULL) {
        return 1;
    }

    //if the astnode contains argument values i.e. its a NODE_COMMAND, we loop through each of the arguments strings and free them, then finally we free the arg_values array itself
    if (node->arg_values != NULL) {
        for (int i = 0; i < node->arg_count; i++) {
            free(node->arg_values[i]);
            node->arg_values[i] = NULL;
        }
        free(node->arg_values);
        node->arg_values = NULL;
    }
    //after freeing the array of string pointers, we set the arg_count to 0
    node->arg_count = 0;

    //now we recursively free the children nodes with the help of the specific pointers with checkers. The check is not necessary as freeing NULL just does nothing, but its still better to read.
    if (node->file_path != NULL) {
        free(node->file_path);
        node->file_path = NULL;
    }

    if (node->child_cmd != NULL) {
        delete_ast_node(node->child_cmd);
        node->child_cmd = NULL;
    }

    if (node->left != NULL) {
        delete_ast_node(node->left);
        node->left = NULL;
    }

    if (node->right != NULL) {
        delete_ast_node(node->right);
        node->right = NULL;
    }

    //once we are sure that all the children nodes, or technically, all the memory address pointerd to by the pointer fields of the structure are freed up, we can free the parent node itself
    free(node);

    //SUCCESS RETURN
    return 1;
}



void print_ast_tree(const ASTNode* node, int depth) {
    //simple check to see if the node was null, in which case do dont do anything
    if (node == NULL) {
        return;
    }

    //a simple spacing logic for recursive printing, that automatically spaces incrementally based on the node level being printed
    for (int i = 0; i < depth; i++) {
        printf("    ");
    }

    //casewise printing of the AST node details
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