#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Token.h"
#include "MemoryGuard.h"

static char* OP_PIPE = "|";
static char* OP_REDIRECT_IN = "<";
static char* OP_REDIRECT_OUT = ">";
static char* OP_REDIRECT_OUT_APPEND = ">>";
static char* OP_BACKGROUND = "&";


Token token_create_word(char* string) {
    //create a new stack variable Token and initialize it to type Word and the value to a heap copy of the string
    Token newWordToken = {
        .type = WORD,
        .value = strdup(string)
    };
    return newWordToken;
}

Token token_create_operator(TokenType tokenType) {
    Token newOperatorToken;
    newOperatorToken.type = tokenType;
    newOperatorToken.value = "";

    switch (tokenType)
    {
    case PIPE:  newOperatorToken.value = OP_PIPE;
        break;
    case REDIRECT_IN:  newOperatorToken.value = OP_REDIRECT_IN;
        break;
    case REDIRECT_OUT:  newOperatorToken.value = OP_REDIRECT_OUT;
        break;
    case REDIRECT_OUT_APPEND:  newOperatorToken.value = OP_REDIRECT_OUT_APPEND;
        break;
    case BACKGROUND:  newOperatorToken.value = OP_BACKGROUND;
        break;
    default: fprintf(stderr, "Unexpected Operator");
        break;
    }
    return newOperatorToken;
}

void delete_token(Token* token) {

    if (token->type == WORD) {
        free(token->value);
        token->value = NULL;
    }
    else {
        //token value are not heap memory pointers, so just null them without freeing
        token->value = NULL;

    }

}

