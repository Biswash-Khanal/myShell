#include <stdlib.h>
#include "Token.h"

Token* create_token(TokenType tokenType, char* value, size_t numChars) {
    Token* newToken = malloc(sizeof(*newToken));

    char* newTokenValue = malloc(sizeof(*newTokenValue) * numChars + 1);

    newToken->type = tokenType;
    newToken->value = newTokenValue;

    return newToken;
}

void delete_token(Token* token) {

    free(token->value);
    token->value = NULL;
    free(token);
    token = NULL;

}

