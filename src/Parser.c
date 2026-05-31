#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Vector.h"
#include "Token.h"
#include "ASTNode.h"
#include "Parser.h"

/**
 * @brief Helper to safely extract a type-safe Token pointer at the current cursor index.
 */
static Token* get_token_pointer(Vector tokenVector, size_t* cursor) {
    // Defend against an unallocated pointer address or an index tracking out of buffer limits
    if (cursor == NULL || *cursor >= tokenVector.length) {
        return NULL;
    }
    // Pull the raw void* address directly via the Vector index and cast to our Token data model
    return (Token*)vec_getItemAddress(&tokenVector, *cursor);
}

/**
 * @brief Translates Lexer Token type enums straight into dedicated AST Node types.
 */
static ASTNodeType map_token_to_node(TokenType tok_type) {
    switch (tok_type) {
    case REDIRECT_IN:         return NODE_REDIRECT_IN;
    case REDIRECT_OUT:        return NODE_REDIRECT_OUT;
    case REDIRECT_OUT_APPEND: return NODE_REDIRECT_APP;
    case BACKGROUND:          return NODE_BACKGROUND;
    case WORD:                return NODE_COMMAND;
    case PIPE:                return NODE_PIPE;
    default:                  exit(EXIT_FAILURE); 
    }
}

/**
 * @brief Master entry point for parsing the abstract token vector.
 */
ASTNode* parseInput(Vector tokenVector) {
    size_t cursor = 0; // The singular source of truth counter for our token stream position

    // Cascade into the top-level grammatical tier (Background/Sequence)
    ASTNode* root = parse_background(tokenVector, &cursor);

    // OPTIMIZATION/GUARD: If the grammar finished processing successfully but unparsed tokens 
    // remain in the buffer array (e.g., "ls & grep"), it represents an illegal syntax layout.
    if (cursor < tokenVector.length) {
        printf("shell: syntax error near unexpected token\n");
        delete_ast_node(root); // Deep free whatever partial tree structures were built to prevent leaks
        return NULL;
    }

    return root; // Return the absolute root element of the finalized execution tree
}


ASTNode* parse_commands(Vector tokenVector, size_t* cursor) {
    // Look up the active token using our shared tracking cursor index
    Token* tok = get_token_pointer(tokenVector, cursor);

    // BASE CASE: If no tokens exist or the active token isn't a WORD, a command cannot begin here.
    if (tok == NULL || tok->type != WORD) {
        return NULL;
    }

    // Capture our current position to run a ultra-fast look-ahead look counter scan
    size_t start = *cursor;
    size_t count = 0;

    // Fast-scan forward to count exactly how many concurrent WORD tokens exist back-to-back
    while (start + count < tokenVector.length) {
        // Drop straight into the vector buffer elements with zero math adjustments
        Token* t = (Token*)vec_getItemAddress(&tokenVector, start + count);
        if (t->type != WORD) {
            break; // Stop counting the moment an operator token interrupts the text stream
        }
        count++;
    }

    // Allocate our new node context
    ASTNode* node = create_ast_node(NODE_COMMAND);
    if (node == NULL) return NULL;

    // and zeros out all slots automatically, naturally establishing the terminating NULL index.
    node->arg_values = calloc(count + 1, sizeof(char*));
    node->arg_count = count;

    // Loop through the precise token count, extracting values and moving the master tracker
    for (size_t i = 0; i < count; i++) {
        // Pull the valid token address matching our current position
        Token* t = get_token_pointer(tokenVector, cursor);

        // Duplicate the heap string payload out of the lexer container into our node list
        node->arg_values[i] = strdup(t->value);

        // Mutate the master cursor directly on the stack to advance past this word
        (*cursor)++;
    }

    return node; // Hand back the complete type-isolated command leaf element
}

/**
 * @brief Grammar Rule 3: Binds file descriptors and path names to an active command leaf.
 */
ASTNode* parse_redirection(Vector tokenVector, size_t* cursor) {
    // Descend into commands first to gather the core target that might be redirected
    ASTNode* left_node = parse_commands(tokenVector, cursor);

    // Get the updated token placement where the command scanning process stopped
    Token* tok = get_token_pointer(tokenVector, cursor);

    // OPTIMIZATION: Early out exit path. If no tokens remain or the current token isn't 
    // a redirect operator, return our left command block exactly as it was built.
    if (tok == NULL || (tok->type != REDIRECT_IN && tok->type != REDIRECT_OUT && tok->type != REDIRECT_OUT_APPEND)) {
        return left_node;
    }

    // SYNTAX GUARD: If we found a redirect operator but left_node is NULL (e.g., "> output.txt"), 
    // the grammar rule is broken. You cannot apply a redirect operator to a non-existent command.
    if (left_node == NULL) {
        printf("shell: syntax error near unexpected token '%s'\n", tok->value);
        return NULL;
    }

    // Create our redirection node container matching the current operator type
    ASTNode* redir_node = create_ast_node(map_token_to_node(tok->type));
    if (redir_node == NULL) return NULL;

    // Parent the valid left-side command execution tree to this new redirection anchor
    redir_node->left = left_node;

    // Consume the operator token and push the shared tracker to the next stream slot
    (*cursor)++;

    // Extract the token that MUST be present directly following the operator symbol
    Token* file_tok = get_token_pointer(tokenVector, cursor);

    // SYNTAX GUARD: If no tokens follow or the next item is an operator instead of a WORD,
    // throw a clear compilation syntax warning. Paths must be valid alpha strings.
    if (file_tok == NULL || file_tok->type != WORD) {
        printf("shell: syntax error near unexpected token 'newline'\n");
        delete_ast_node(redir_node); // Free the invalid redirection wrapper alongside its child command
        return NULL;
    }

    // Duplicate the target string value into our dedicated file path container
    redir_node->file_path = strdup(file_tok->value);

    // Consume the target filename token to clean our stream positioning
    (*cursor)++;

    return redir_node; // Return our completed tree-wrapped redirection node frame
}

/**
 * @brief Grammar Rule 2: Combines distinct execution blocks together via pipe linkages.
 * OPTIMIZATION: Flattened nested logic and transformed right-recursion patterns into a high-performance linear loop.
 */
ASTNode* parse_pipeline(Vector tokenVector, size_t* cursor) {
    // Lower down into our redirection step to establish our foundational left-side operand
    ASTNode* left_node = parse_redirection(tokenVector, cursor);

    // Run a linear loop to smoothly group multi-stage pipelines from left-to-right (e.g., A | B | C)
    while (1) {
        // Query the updated state pointer of our token sequence
        Token* tok = get_token_pointer(tokenVector, cursor);

        // Break out of our looping layer the second we hit the line end or find a non-pipe token
        if (tok == NULL || tok->type != PIPE) {
            break;
        }

        // SYNTAX GUARD: If a pipe symbol is discovered but our left expression tree is completely 
        // blank, we've encountered a structural layout failure (e.g., "| grep test" or "cat a > | wc")
        if (left_node == NULL) {
            printf("shell: syntax error near unexpected token '|'\n");
            return NULL;
        }

        // Consume the matching PIPE operator token out of the processing queue
        (*cursor)++;

        // Create the active branch node to track this operational phase
        ASTNode* pipe_node = create_ast_node(NODE_PIPE);
        if (pipe_node == NULL) {
            delete_ast_node(left_node);
            return NULL;
        }

        // Assign our current left tree state directly to the left leg of this new operator node
        pipe_node->left = left_node;

        // Parse the right side of the pipe by recursively invoking the redirection cascade step
        pipe_node->right = parse_redirection(tokenVector, cursor);

        // SYNTAX GUARD: If the right-hand operand fails to parse (e.g., "ls |"), we've found
        // a trailing pipeline syntax dead-end. Terminate the process cleanly.
        if (pipe_node->right == NULL) {
            printf("shell: syntax error near unexpected token '|'\n");
            delete_ast_node(pipe_node); // Deep clean our entire multi-tier tree tracking matrix
            return NULL;
        }

        // OPTIMIZATION: Shift our pointer perspective. This newly completed pipe frame 
        // becomes the left-hand foundation node for any subsequent loop passes.
        left_node = pipe_node;
    }

    return left_node; // Return our unified multi-stage pipeline command matrix back up the stack
}

/**
 * @brief Grammar Rule 1: Scans the master sequence to identify async background operators.
 * OPTIMIZATION: Stripped out redundant fallback check loops.
 */
ASTNode* parse_background(Vector tokenVector, size_t* cursor) {
    // Call into the pipeline processor to gather the foundational execution hierarchy
    ASTNode* left_node = parse_pipeline(tokenVector, cursor);

    // Inspect the current token marker where the sub-cascade operations yielded control
    Token* tok = get_token_pointer(tokenVector, cursor);

    // If no extra tokens exist on the tail end of the stream, return our tree structure unmodified
    if (tok == NULL || tok->type != BACKGROUND) {
        return left_node;
    }

    // If the token matches our BACKGROUND token symbol ('&')
    // Consume the background operator token out of our queue tracker
    (*cursor)++;

    // Construct the top-tier background execution node structure
    ASTNode* bg_node = create_ast_node(NODE_BACKGROUND);
    if (bg_node == NULL) {
        delete_ast_node(left_node); // Protect our memory barriers if allocation errors trigger
        return NULL;
    }

    // Anchor our primary command tree safely to the left child leg of this async structural envelope
    bg_node->left = left_node;

    return bg_node; // Return our top-level execution tree container back to the parser gateway
}