#ifndef ASTNODE_H
#define ASTNODE_H


typedef enum {
    NODE_COMMAND,       // Represents a full executable program with all its arguments
    NODE_PIPE,          // Represents the PIPE '|' operator link
    NODE_REDIRECT_OUT,  // Represents '>'
    NODE_REDIRECT_APP,  // Represents '>>'
    NODE_REDIRECT_IN,   // Represents '<'
    NODE_BACKGROUND     // Represents '&'
} ASTNodeType;


typedef struct ASTNode ASTNode;

typedef struct ASTNode {
    ASTNodeType type; 

    
    char** arg_values;         
    int arg_count;


    ASTNode* left;
    ASTNode* right;


    ASTNode* child_cmd;
    char* file_path;    
} ASTNode;



ASTNode* create_ast_node(ASTNodeType nodeType);


int delete_ast_node(ASTNode* node);



void print_ast_tree(const ASTNode* node, int depth);

#endif