#include <stdlib.h>

#include "ASTNode.h"

ASTNode* astNode_create(ASTNodeType nodeType) {

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


    //loop through and free all the command streings individually
    for (int i = 0; i < node->arg_count;i++) {
        free(*(node->arg_values + i));
        //not necessary, just setting a habbit
        *(node->arg_values + i) = NULL;
    }

    //then free the parent array
    free(node->arg_values);
    node->arg_values = NULL;

    //zero the arg count
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



