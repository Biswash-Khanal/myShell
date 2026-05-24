#include "Vector.h"
#include "Token.h"

#include <ctype.h>

int isOperator(char inputCharacter) {
    if (inputCharacter == '|' || inputCharacter == '<' || inputCharacter == '>' || inputCharacter == '&') {
        return 1;
    }
    else {
        return 0;
    }
}

int isCharacter(char inputCharacter) {
    if (!isspace(inputCharacter) && inputCharacter != '\0' && isOperator(inputCharacter) != 1) {
        return 1;
    }
    else {
        return 0;
    }
}

void lexer(char* inputstring, Vector* vector) {
    char* ptr = inputstring;
    Token newToken;

    while (*ptr != '\0' && *ptr != '\n') {

        // Prtotect from all the spaces/tabs immediately
        // Protect \n and \0 so we dont blow past our main loop boundaries
        if (isspace(*ptr) && *ptr != '\n') {
            ptr++;
            continue; // Skip straight to the next loop step
        }

        //(now we are sure to be non whitespace)
        switch (*ptr) {
        case '|':
            newToken = token_create_operator(PIPE);
            vec_pushBack(vector, &newToken);
            break;
        case '<':
            newToken = token_create_operator(REDIRECT_IN);
            vec_pushBack(vector, &newToken);
            break;
        case '>':
            if (*(ptr + 1) == '>') {
                newToken = token_create_operator(REDIRECT_OUT_APPEND);
                vec_pushBack(vector, &newToken);
                ptr++;
            }
            else {
                newToken = token_create_operator(REDIRECT_OUT);
                vec_pushBack(vector, &newToken);
            }
            break;
        case '&':
            newToken = token_create_operator(BACKGROUND);
            vec_pushBack(vector, &newToken);
            break;

        default:
            //default case is the WORD token case
        {
            //set a pointer to the start of the word
            char* wordStart = ptr;

            // iterate the pointer until a character that ends a word is found like a whitespace or an operator
            while (isCharacter(*ptr)) {
                ptr++;
            }

            // Temporarily add \0 to terminate the word string while saving a backup of the original character
            char originalChar = *ptr;
            *ptr = '\0';

            // Create and store the word token
            newToken = token_create_word(wordStart);
            vec_pushBack(vector, &newToken);

            // Restore the original character
            *ptr = originalChar;

            //Need to continue here, because the logic itself advanced the pointer appropriately already, else the pointer would advance 1 eextra time
            continue;
        }
        break;
        }

        ptr++; // Moves to the next character for operators
    }
}