#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#include "Token.h"
#include "Vector.h"
#include "MemoryGuard.h"

//Storing strings for each operator in static memory, so we can simply give its reference when creating new tokens.
static char* OP_PIPE = "|";
static char* OP_REDIRECT_IN = "<";
static char* OP_REDIRECT_OUT = ">";
static char* OP_REDIRECT_OUT_APPEND = ">>";
static char* OP_BACKGROUND = "&";


Token token_create_word(char* string) {

    //Create and initialize a new token with type WORD, and value a strdup'd string.
    Token newWordToken = {
        .type = WORD,
        .value = strdup(string)
    };

    //then we return the new created WORD token
    return newWordToken;
}

Token token_create_operator(TokenType tokenType) {

    // Token newOperatorToken;
    // newOperatorToken.type = tokenType;
    // newOperatorToken.value = "";

    //Create and initialize a new token with the type that was passed, and an initial value of NULL, later to be casewise assigned.
    Token newOperatorToken = {
        .type = tokenType,
        .value = NULL
    };

    //Assign different value(the static strings) based on the tokenType
    switch (tokenType)
    {
    case PIPE:  newOperatorToken.value = OP_PIPE;                               break;
    case REDIRECT_IN:  newOperatorToken.value = OP_REDIRECT_IN;                 break;
    case REDIRECT_OUT:  newOperatorToken.value = OP_REDIRECT_OUT;               break;
    case REDIRECT_OUT_APPEND:  newOperatorToken.value = OP_REDIRECT_OUT_APPEND; break;
    case BACKGROUND:  newOperatorToken.value = OP_BACKGROUND;                   break;
    default: fprintf(stderr, "Unexpected Operator");                            break;
    }

    //then we return the newly created operator token.
    return newOperatorToken;
}

void delete_token(Token* token) {

    //if the token type was WORD, we can simply free the string malloced by strdup, and then set it to NULL
    if (token->type == WORD) {
        free(token->value);
        token->value = NULL;
    }
    //If the token was one of the operators, we dont need to free the value anymore, we can just assign it NULL
    else {
        token->value = NULL;
    }

}

void freeTokenVector(Vector* tokenVector) {

    //get the pointer to the start of the buffer of the token vector
    /***NOTE TO SELF, BECAUSE I MADE THE RETURN TYPE OF THE BUFFER ADDRESS GETTER TO BE A CONST, WE CANT USE IT HERE, NEED TO RETHINK */
    Token* ptr = vec_getItemAddress(tokenVector, 0);

    //loop through each element of the vector and run the delete token function for each of them
    for (size_t i = 0;i < tokenVector->length;i++) {

        delete_token(ptr);

        ptr++;
    }

    //finally, once each individual tokens are freed, we can run the generic vector delete function
    vec_deleteVector(tokenVector);

}


void printVectorBuffer(const Vector* tokenVector) {
    //check for valid vector
    if (tokenVector == NULL || tokenVector->length == 0) {
        printf("Vector Length = 0\n");
        return;
    }

    //get the address of the vector buffer
    Token* ptr = vec_getItemAddress((Vector*)tokenVector, 0);

    //print the vector length
    printf("Vector Length = %lu: [ ", tokenVector->length);

    //loop through the vector, and for each token, casewise switch to print the token type and the value accordingly.
    for (size_t i = 0; i < tokenVector->length; i++) {

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


        if (i < tokenVector->length - 1) {
            printf(" -> ");
        }
        ptr++;
    }
    printf(" ]\n");
}