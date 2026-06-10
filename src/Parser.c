#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Vector.h"
#include "Token.h"
#include "ASTNode.h"
#include "Parser.h"


static Token* get_token_pointer(Vector tokenVector, size_t* cursor) {

    if (cursor == NULL || *cursor >= tokenVector.length) {
        return NULL;
    }

    return (Token*)vec_getItemAddress(&tokenVector, *cursor);
}


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

ASTNode* parseInput(Vector tokenVector) {
    size_t cursor = 0; 

  
    ASTNode* root = parse_background(tokenVector, &cursor);

   

    if (cursor < tokenVector.length) {
        printf("shell: syntax error near unexpected tokens\n");
        delete_ast_node(root); 
        return NULL;
    }

    return root; 
}


ASTNode* parse_commands(Vector tokenVector, size_t* cursor) {
    
    Token* tok = get_token_pointer(tokenVector, cursor);

    
    if (tok == NULL || tok->type != WORD) {
        return NULL;
    }

    
    size_t start = *cursor;
    size_t count = 0;

    
    while (start + count < tokenVector.length) {
        
        Token* t = (Token*)vec_getItemAddress(&tokenVector, start + count);
        if (t->type != WORD) {
            break; 
        }
        count++;
    }

   
    ASTNode* node = create_ast_node(NODE_COMMAND);
    if (node == NULL) return NULL;

    node->arg_values = calloc(count + 1, sizeof(char*));
    node->arg_count = count;

    for (size_t i = 0; i < count; i++) {
        Token* t = get_token_pointer(tokenVector, cursor);

        node->arg_values[i] = strdup(t->value);

        (*cursor)++;
    }

    return node; 
}


ASTNode* parse_redirection(Vector tokenVector, size_t* cursor) {
    ASTNode* current_node = parse_commands(tokenVector, cursor);

    while (1) {
        Token* tok = get_token_pointer(tokenVector, cursor);

        if (tok == NULL || (tok->type != REDIRECT_IN &&
            tok->type != REDIRECT_OUT &&
            tok->type != REDIRECT_OUT_APPEND)) {
            break;
        }

        if (current_node == NULL) {
            printf("shell: syntax error near unexpected token '%s'\n", tok->value);
            return NULL;
        }

        ASTNode* redir_node = create_ast_node(map_token_to_node(tok->type));
        if (redir_node == NULL) {
            delete_ast_node(current_node);
            return NULL;
        }

        redir_node->left = current_node;

        (*cursor)++;

        Token* file_tok = get_token_pointer(tokenVector, cursor);

        if (file_tok == NULL || file_tok->type != WORD) {
            printf("shell: syntax error near unexpected token 'newline'\n");
            delete_ast_node(redir_node); 
            return NULL;
        }

        
        redir_node->file_path = strdup(file_tok->value);

        
        (*cursor)++;

        
        
        current_node = redir_node;
    }

    return current_node; 
}


ASTNode* parse_pipeline(Vector tokenVector, size_t* cursor) {
    ASTNode* left_node = parse_redirection(tokenVector, cursor);

    while (1) {
        Token* tok = get_token_pointer(tokenVector, cursor);

        if (tok == NULL || tok->type != PIPE) {
            break;
        }

        if (left_node == NULL) {
            printf("shell: syntax error near unexpected token '|'\n");
            return NULL;
        }

        (*cursor)++;

        ASTNode* pipe_node = create_ast_node(NODE_PIPE);
        if (pipe_node == NULL) {
            delete_ast_node(left_node);
            return NULL;
        }

        pipe_node->left = left_node;

        pipe_node->right = parse_redirection(tokenVector, cursor);

        if (pipe_node->right == NULL) {
            printf("shell: syntax error near unexpected token '|'\n");
            delete_ast_node(pipe_node); 
            return NULL;
        }


        left_node = pipe_node;
    }

    return left_node; 
}


ASTNode* parse_background(Vector tokenVector, size_t* cursor) {
    ASTNode* left_node = parse_pipeline(tokenVector, cursor);


    Token* tok = get_token_pointer(tokenVector, cursor);


    if (tok == NULL || tok->type != BACKGROUND) {
        return left_node;
    }


    (*cursor)++;

    
    ASTNode* bg_node = create_ast_node(NODE_BACKGROUND);
    if (bg_node == NULL) {
        delete_ast_node(left_node); 
        return NULL;
    }

   
    bg_node->left = left_node;

    return bg_node; 
}