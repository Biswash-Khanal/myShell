#ifndef TOKEN_H
#define TOKEN_H

#include "Vector.h"


typedef enum TokenType {
    WORD,

    REDIRECT_IN,
    REDIRECT_OUT,
    REDIRECT_OUT_APPEND,
    PIPE,
    BACKGROUND,
} TokenType;


typedef struct Token {
    TokenType type;
    char* value;
} Token;


Token token_create_word(char* string);



Token token_create_operator(TokenType tokenType);



void delete_token(Token* token);


void freeTokenVector(Vector* tokenVector);


void printVectorBuffer(const Vector* vector);
#endif