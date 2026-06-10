#include <ctype.h>
#include <stdio.h>

#include "Vector.h"
#include "Token.h"


int isOperator(char inputCharacter) {
    if (inputCharacter == '|' || inputCharacter == '<' || inputCharacter == '>' || inputCharacter == '&') {
        return 1;
    }
    else {
        return 0;
    }
}


int isCharacter(char inputCharacter) {
    // A character belongs to a regular word ONLY if it's:
    // 1. Not whitespace
    // 2. Not the string null terminator
    // 3. Not an operator (| , < , > , &)
    // 4. Not a quotation mark (" or ')
    if (!isspace((unsigned char)inputCharacter) &&
        inputCharacter != '\0' &&
        inputCharacter != '"' &&
        inputCharacter != '\'' && // CRITICAL FIX: Single quote also breaks a regular word boundary
        isOperator(inputCharacter) != 1) {
        return 1;
    }
    else {
        return 0;
    }
}


int lexer(char* inputstring, Vector* vector) {

    char* ptr = inputstring;
    Token newToken;


    while (*ptr != '\0' && *ptr != '\n') {

        if (isspace((unsigned char)*ptr) && *ptr != '\n' && *ptr != '\0') {
            ptr++;
            continue; 
        }


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

        case '"':
        case '\'': {
            char quoteChar = *ptr; 
            ptr++; 

            if (*ptr == quoteChar) {
                newToken = token_create_word(""); 
                vec_pushBack(vector, &newToken);
                ptr++; 
                continue;
            }

            char* wordStart = ptr;

            while (*ptr != quoteChar) {

                if (*ptr == '\0' || *ptr == '\n') {
                    fprintf(stderr, "myshell: syntax error: unclosed quotation mark\n");
                    return -1; // Failure status
                }

                ptr++;
            }

            char originalChar = *ptr;
            *ptr = '\0';

            newToken = token_create_word(wordStart);
            vec_pushBack(vector, &newToken);

            *ptr = originalChar; 
            ptr++;               
            continue;
        }
              
        default: {
            
            char* wordStart = ptr;
            while (isCharacter(*ptr)) {
                ptr++;
            }

            char originalChar = *ptr;
            *ptr = '\0';

            newToken = token_create_word(wordStart);
            vec_pushBack(vector, &newToken);

            *ptr = originalChar;
            continue;
        }
               break;

        }

        ptr++;
    }
    
    return 0;
}