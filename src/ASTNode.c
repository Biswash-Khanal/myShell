#include <stdlib.h>

#include "ASTNode.h"

ASTNode* astNode_create(ASTNodeType nodeType) {

    ASTNode* newNode = malloc(sizeof(*newNode));
    if (newNode == NULL) {
        return NULL;
    }

    newNode->type = nodeType;

    newNode->right = NULL;
    newNode->left = NULL;


    newNode->file_path = NULL;
    newNode->child_cmd = NULL;


    newNode->args = NULL;
    newNode->arg_count = 0;

    return newNode;
}

int delete_ast_node(ASTNode* node) {
    if (node == NULL) {
        return 0;
    }

    //i can either go case by case and only free thje properties that are used for the specific case
    switch (node->type) {
    case NODE_PIPE:
        node->left = NULL;
        node->right = NULL;
        break;
    }
}

