#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#include "Token.h"
#include "Vector.h"
#include "MemoryGuard.h"


static char* OP_PIPE = "|";
static char* OP_REDIRECT_IN = "<";
static char* OP_REDIRECT_OUT = ">";
static char* OP_REDIRECT_OUT_APPEND = ">>";
static char* OP_BACKGROUND = "&";


Token token_create_word(char* string) {

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

        token->value = NULL;

    }

}

voidfreeTokenVector(Vector* tokenVector) {


    Token* ptr = vec_getItemAddress(tokenVector, 0);


    for (size_t i = 0;i < tokenVector->length;i++) {

        delete_token(ptr);

        ptr++;
    }


    vec_deleteVector(tokenVector);

}


void printVectorBuffer(const Vector* vector) {
    if (vector == NULL || vector->length == 0) {
        printf("Vector Length = 0\n");
        return;
    }


    Token* ptr = vec_getItemAddress((Vector*)vector, 0);

    printf("Vector Length = %lu: [ ", vector->length);

    for (size_t i = 0; i < vector->length; i++) {

        switch (ptr->type) {
        case WORD:
            printf("WORD(\"%s\")", ptr->value);
            break;
        case REDIRECT_IN:
            printf("REDIRECT_IN"); 
            break;
        case REDIRECT_OUT:
            printf("REDIRECT_OUT");
            break;
        case REDIRECT_OUT_APPEND:
            printf("REDIRECT_OUT_APPEND");
            break;
        case PIPE:
            printf("PIPE");
            break;
        case BACKGROUND:
            printf("BACKGROUND");
            break;
        default:
            printf("UNKNOWN_TOKEN");
            break;
        }


        if (i < vector->length - 1) {
            printf(" -> ");
        }
        ptr++;
    }
    printf(" ]\n");
}