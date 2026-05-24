#ifndef TOKEN_H
#define TOKEN_H

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

void delete_token(Token* token);

#endif