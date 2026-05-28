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