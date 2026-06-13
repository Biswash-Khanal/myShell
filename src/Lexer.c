#include <ctype.h>
#include <stdio.h>

#include "Vector.h"
#include "Token.h"

/**
 * @brief Helper function that classifies if a character is an operator or not.
 * 
 * @param inputCharacter The character to be tested.
 * @return int Return 1 if the character is an operator, 0 if its not.
 */
static int isOperator(char inputCharacter) {
    if (inputCharacter == '|' || inputCharacter == '<' || inputCharacter == '>' || inputCharacter == '&') {
        return 1;
    }
    else {
        return 0;
    }
}

/**
 * @brief Helper function that classifies if a character is a valid WORD character i.e not OPERATOR, not WHITESPACE, not '\0', not "" or ''.
 * 
 * @param inputCharacter The character to be tested.
 * @return int Returns 1 if the character is valid, 0 if not.
 */
static int isCharacter(char inputCharacter) {
    // A character belongs to a regular word ONLY if it's:
    // 1. Not whitespace
    // 2. Not the string null terminator
    // 3. Not an operator (| , < , > , &)
    // 4. Not a quotation mark (" or ')
    if (!isspace((unsigned char)inputCharacter) &&
        inputCharacter != '\0' &&
        inputCharacter != '"' &&
        inputCharacter != '\'' &&
        isOperator(inputCharacter) != 1) {
        return 1;
    }
    else {
        return 0;
    }
}


int lexer(char* inputstring, Vector* vector) {

    char* ptr = inputstring;            //Store the pointer on a shorter easier to use variable ptr.
    Token newToken;                     //We create a new Token variable, that will be used in each iteration 


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