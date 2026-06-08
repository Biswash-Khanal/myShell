#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Vector.h"
#include "Token.h"
#include "ASTNode.h"
#include "Parser.h"

/**
 * @brief Recursive Descent Parsing Technique.
 * Basic gist of the algorithm is, we first decide the operator precedence and priority. like if a+b*c+d, maybe we can have a rule to first do the additions and then multiply, or vice versa.
 * In our case, the priority is kept simple for this first demo. I think it will be very complicated and ill never finish the project if i keep trying to improve before even finishing something.
 * So, my operator priorities are simple
 * Background operator happens at last no matter what, sub commands are not allowed to be background, in other words, only valid place for the background operator to be is at the end of the command. Anywhere else will be error. And basically the entire command before the & operator is now treated as the command to be ran in bacground.
 *
 * Next is the Pipe operator. As the pipe operator is supposed to just take the output from one of the commands and pass it as the input into another command, its logical that among the remaining operator its the highest priority or the last one to be operated.
 *
 * Next, is the redirection operators. I like to call these pseudo operators, as its not really operating anything, all they do is change the default file stream pointers. However, logically, the F pointers are changed in order to redirect the input or the output stream FOR a COMMAND and so it makes sense this needs to happen AFTER a command is decoded.
 *
 * Finally, we have the WORDS, these are basically each seperate words. I have decided all commands will be nothing else but an array or strings. the first string being the main command name, or a subprogram and the rest being the arguments for it to function. **im saying I am doing all this but its all the actual practices, ive just toned then down and made them simpler for this simple learning project
 *
 * How the parsing works, is basically we can converting the serial token vectors into a tree structure, the tree structure follows the following rules
 *
 * for background operator, we make a background node, and store a pointer to the entire rest of the tree within it
 * for pipe operator, we make a PIPE node, a left pointer and a right pointer, which respectively store the pointer to the sub command nodes
 * for redirection operator, we make the specific redirection node with a pointer to the node containing the actual command
 * and finally we have the actual command node, that stores a sequence of valid words as its arguments and keeps track of the count as well
 *
 * we can also observe from this explanation that this tree will have the command nodes as it's leaves. If present, redirection nodes will contain the command nodes, and so on until the top
 *
 * Now, there are a lot of edge cases, error detections, sequence of tokens validation stuff for it to be functioning properly, however given the purpose of the project and not to be sidetracked, ive just plugged the edge cases and problems as they arise instead of thinking through and programming a general solution. So, that is a TODO
 *
 * As to how the actual code is working, well we throw in the root node  into the parsing function, which is like an entry point, it will be responsible for starting the process of parsing, and when the process completes it can do some checks to verify everything worked smoothely. It can use the return values of the daughter functions and some other logics to figure out if everything went correctly. It also then passes the tokenvector to the parse_background function
 *
 * parse_background function immediately calls the next in line function the parse_pipeline. And as we can see this pattern of calling the other function immediately carrys on until the final function that is the parse_command function. What this allows is at the start of parsing and infact in each step of parsing, RECURSIVELY, each token is always sure to at least visit the correct parsing function once. It will be hard to explain in words and long as well, but ive tried to comment and write the code clean enough so its very clear whats happening.
 *
 *
 * basically, each level function when gets called, sets up the token into the proper node if the fuction matches the token type, else just returns a level up, allowing next in line function to check if the token type matches, and with this flow, we can also add problem chcking logics which makes sure if the command is valid it parses correctly, or if something is wrong, we can let the mother function parse_input know what went wrong and send error messages. Currently, i have not setup specific error checking and exact error messages, for now it will just display a generic error message if something went wrong.
 */

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
        //this is getting hit with cat <hello.txt >>newFile.txt
        printf("shell: syntax error near unexpected tokens\n");
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
    // Lower down into commands first to gather our core baseline execution target
    ASTNode* current_node = parse_commands(tokenVector, cursor);

    // Run a loop to accumulate redirection layers as long as they appear back-to-back
    while (1) {
        // Query the active token placement where the last step stopped
        Token* tok = get_token_pointer(tokenVector, cursor);

        // Break out of our looping layer the second we hit the line end or find a non-redirect token
        if (tok == NULL || (tok->type != REDIRECT_IN &&
            tok->type != REDIRECT_OUT &&
            tok->type != REDIRECT_OUT_APPEND)) {
            break;
        }

        // SYNTAX GUARD: If a redirect symbol is discovered but our target node is empty
        // (e.g., "> output.txt"), the grammar rules are broken.
        if (current_node == NULL) {
            printf("shell: syntax error near unexpected token '%s'\n", tok->value);
            return NULL;
        }

        // Create our new redirection node container matching this specific operator iteration
        ASTNode* redir_node = create_ast_node(map_token_to_node(tok->type));
        if (redir_node == NULL) {
            delete_ast_node(current_node);
            return NULL;
        }

        // Parent our accumulated tree layout safely to the left child leg of this new operator node
        redir_node->left = current_node;

        // Consume the operator token out of the processing queue
        (*cursor)++;

        // Extract the token that MUST be present directly following the operator symbol
        Token* file_tok = get_token_pointer(tokenVector, cursor);

        // SYNTAX GUARD: File paths must be valid alpha-string word tokens
        if (file_tok == NULL || file_tok->type != WORD) {
            printf("shell: syntax error near unexpected token 'newline'\n");
            delete_ast_node(redir_node); // Free tree and parent frames safely
            return NULL;
        }

        // Duplicate the target string value into our dedicated file path container
        redir_node->file_path = strdup(file_tok->value);

        // Consume the target filename token to advance our stream position
        (*cursor)++;

        // SHIFT PERSPECTIVE: This newly wrapped redirection shell becomes the 
        // target node for any subsequent redirection matches in the next loop pass.
        current_node = redir_node;
    }

    return current_node; // Return our completely stacked redirection tree frame
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