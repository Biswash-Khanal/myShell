#ifndef TOKEN_H
#define TOKEN_H



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

Token* create_token(TokenType tokenType, char* value, size_t numChars);
void delete_token(Token* token);



#endif