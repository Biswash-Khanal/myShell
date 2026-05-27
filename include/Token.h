#ifndef TOKEN_H
#define TOKEN_H

#include "Vector.h"

/**
 * @brief enum to hold the different token types
 *
 */
typedef enum TokenType {
    WORD,

    REDIRECT_IN,
    REDIRECT_OUT,
    REDIRECT_OUT_APPEND,
    PIPE,
    BACKGROUND,
} TokenType;

/**
 * @brief Token datatype that stores the token type and the token value
 *
 */
typedef struct Token {
    TokenType type;
    char* value;
} Token;

/**
 * @brief Create a token for WORD
 *
 * @param value String value of the token (pointer to the heap containing the heap)
 * @return Token
 */
Token token_create_word(char* string);


/**
 * @brief Create a token for Operator
 *
 * @param tokenType The type of token
 * @return Token
 */
Token token_create_operator(TokenType tokenType);


/**
 * @brief Frees the Token appropriately based on token
 *
 * @param token Token to be freed
 */
void delete_token(Token* token);

/**
 * @brief Systematically frees a Vector of Token data types by first looping through each tokens, freeing individual string memory, then the individual token memory and finally the vecotr buffer itself and then nullifies the vector data
 *
 * @param tokenVector The Vector or tokens to be freed
 */
void freeTokenVector(Vector* tokenVector);

/**
 * @brief A utility function to display the contents of a Vector of token datatype. Used for debugging and showing if token vector is storing correct data
 *
 * @param vector The vector of tokens to be displayed
 */
void printVectorBuffer(const Vector* vector);
#endif