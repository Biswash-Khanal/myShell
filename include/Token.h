#ifndef TOKEN_H
#define TOKEN_H

#include "Vector.h"

/**
 * @brief The enum that stores the type of the token.
 * 
 */
typedef enum TokenType {
    //Type of the token is a WORD
    WORD,
    //Type of the token is one of the operators.
    REDIRECT_IN,
    REDIRECT_OUT,
    REDIRECT_OUT_APPEND,
    PIPE,
    BACKGROUND,
} TokenType;

//Create a new Datatype that containes the type of the token and the value of the token.
typedef struct Token {
    TokenType type;     //token Type : Enum
    char* value;        //token Value: String
} Token;


/**
 * @brief Creates a new token of WORD type.
 * 
 * @param string The value of the WORD token.
 * @return Token Returns the created token.
 */
Token token_create_word(char* string);


/**
 * @brief Creates a new token of one of the OPERATORS type.
 * 
 * @param tokenType The type of the token to be created(Enum).
 * @return Token Returns the created token.
 */
Token token_create_operator(TokenType tokenType);


/**
 * @brief Frees up memory used by and nullifies, the values in a token.
 * 
 * @param token The token to be freed.
 */
void delete_token(Token* token);

/**
 * @brief Frees up a vector or data type token properly.
 * 
 * @param tokenVector The vector that stores Token type elements that needs to be freed.
 */
void freeTokenVector(Vector* tokenVector);

/**
 * @brief Prints the vector of token vectors in an understandable way for debugging and visualizing.
 * 
 * @param vector The vector that stores Token type elements that needs to be printed.
 */
void printVectorBuffer(const Vector* tokenVector);
#endif