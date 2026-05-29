typedef enum {
    NODE_COMMAND,       // Represents a full executable program with all its arguments
    NODE_PIPE,          // Represents the PIPE '|' operator link
    NODE_REDIRECT_OUT,  // Represents '>'
    NODE_REDIRECT_APP,  // Represents '>>'
    NODE_REDIRECT_IN,   // Represents '<'
    NODE_BACKGROUND     // Represents '&'
} ASTNodeType;

//Forward reference it so that it can be used cleanly inside.
typedef struct ASTNode ASTNode;

typedef struct ASTNode {
    ASTNodeType type; // Common property for all cases. The type of the AST Node

    // --- CASE 1: If type == NODE_COMMAND ---
    // We swallow the values of multiple WORD tokens into a clean string array!
    char** args;         // e.g., args[0] = "ls", args[1] = "-la", args[2] = "/tmp", args[3] = NULL
    int arg_count;

    // --- CASE 2: If type == NODE_PIPE ---
    // The node acts as a pure structural bridge
    ASTNode* left;
    ASTNode* right;

    // --- CASE 3: If type == NODE_REDIRECT_OUT / IN / APP ---
    // The node tracks the command being altered, and the file name string
    ASTNode* child_cmd;
    char* file_path;     // The parsed filename string pulled from a WORD token
} ASTNode;


/**
 *@brief Create a ast node object
 *
 * @param nodeType The type of node
 * @return ASTNode* The pointer to the created node, or null if error
 */
ASTNode* create_ast_node(ASTNodeType nodeType);

/**
 *@brief deletes individual node. Can be used independently but not recommended. used as a utility function for the delete_ast_node function.
 * 
 * @param node the node to be deleted 
 * @return int 
 */
int delete_ast_node(ASTNode* node);


/**
 *@brief frees all the dependent nodes and then finally frees itself
 *
 * @param rootNode the pointer to the root node
 * @return int
 */
int delete_ast_tree(ASTNode* rootNode);

