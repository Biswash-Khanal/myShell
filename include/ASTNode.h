#ifndef ASTNODE_H
#define ASTNODE_H

//The enum to store the different types of the AST nodes.
typedef enum {
    NODE_COMMAND,       // Represents a full executable program with all its arguments
    NODE_REDIRECT_APP,  // Represents '>>'
    NODE_REDIRECT_OUT,  // Represents '>'
    NODE_REDIRECT_IN,   // Represents '<'
    NODE_PIPE,          // Represents the PIPE '|' operator link
    NODE_BACKGROUND     // Represents '&'
} ASTNodeType;

//Forward declaration of typedef to be able to use ASTNode* directly instead of struct ASTNode*

//The struct that works as the AST node datatype.
typedef struct ASTNode ASTNode;

struct ASTNode {
    //variables common to any Node Type.    
    ASTNodeType type;   //The type of the AST node. (ASTNodeType)

    //variables used by NODE_COMMAND
    char** arg_values; //array of strings that store each of the command elements i.e the command and its arguments.
    int arg_count;      //the number of arguments including the command.

    //variables used by NODE_PIPE

    ASTNode* left;      //pointer to the left ASTNode for the pipe
    ASTNode* right;     //pointer to the right ASTNode for the pipe

    //variables used by NODE_REDIRECT_IN
    //variables used by NODE_REDIRECT_OUT
    //variables used by NODE_REDIRECT_APP

    ASTNode* child_cmd;  //The node after the Redirection operator.
    char* file_path;     //The file name/path combined with the redirection operator that is to replace the std streams.
};

/**
 * @brief Create a ast node object
 * 
 * @param nodeType The type of the node to be created.
 * @return ASTNode* Returns the created node.
 */
ASTNode* create_ast_node(ASTNodeType nodeType);

/**
 * @brief Deletes the ast node object.
 * 
 * @param node The node that along with its children is to be deleted.
 * @return int The delete status.
 */
int delete_ast_node(ASTNode* node);


/**
 * @brief Visualize and print the AST tree for debugging and visual analysis of the AST tree format.
 * 
 * @param node The AST node to be printed.
 * @param depth How deep the difference in levels is to be visualized as.
 */
void print_ast_tree(const ASTNode* node, int depth);

#endif