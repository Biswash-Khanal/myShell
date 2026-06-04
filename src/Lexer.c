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
    // 4. Not a quotation mark (")
    if (!isspace((unsigned char)inputCharacter) &&
        inputCharacter != '\0' &&
        inputCharacter != '"' &&
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

        // Prtotect from all the spaces/tabs immediately
        // Protect \n and \0 so we dont blow past our main loop boundaries
        if (isspace(*ptr) && *ptr != '\n' && *ptr != '\0') {
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

            //A bit long and complex case TODO: a seperate function just for this case
        case '"': {
            ptr++; // Step past the opening quote

            // Handle Edge Case: Immediate empty quotes ("")
            if (*ptr == '"') {
                newToken = token_create_word(""); // Stores an empty string literal
                vec_pushBack(vector, &newToken);
                ptr++; // Step past the closing quote
                continue;
            }

            char* wordStart = ptr;

            // Search for the matching closing quote
            while (*ptr != '"') {
                // If we hit the end of the line without finding a closing quote
                if (*ptr == '\0' || *ptr == '\n') {
                    fprintf(stderr, "myshell: syntax error: unclosed quotation mark\n");
                    return 0; // Failure status
                }
                // Operators and spaces are allowed to safely bypass here as strings!
                ptr++;
            }

            // Temporarily set '\0' at the end of the word inside quotes, and pass the wordstart pointer so it treats the substring as the string
            char originalChar = *ptr;
            *ptr = '\0';

            newToken = token_create_word(wordStart);
            vec_pushBack(vector, &newToken);

            *ptr = originalChar; // Restore the closing quote character
            ptr++;               // Step past the closing quote safely
            continue;
        }
                //END of CASE: '"'


        default: {
            //same logic as the quotation, just with added guards for whitepace
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
    return 1; // Success status
}