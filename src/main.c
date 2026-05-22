#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


//the maximum buffer size in bytes 1024 for now
#define  MAX_BUFFER_SIZE 1024

char buffer[MAX_BUFFER_SIZE];

typedef enum TokenType {
    WORD,
    PIPE,
    REDIR_IN,
    REDIR_OUT,
    REDIR_OUT_APPEND,
    BACKGROUND,
    NEWLINE

} TokenType;


typedef enum LexerState {
    NORMAL,
    IS_WORD,
    IS_OPERATOR
} LexerState;

typedef struct Token
{
    TokenType type;
    char* value;
} Token;

char peek(char* currentPtr) {
    return *(currentPtr + 1);
}


//a rough idea for a lexer
void lexer(char* inputString) {
    LexerState currentState = NORMAL;

    while (1) {
        switch (currentState) {

        case NORMAL:
            switch (*inputString) {
                
            case '|':
                //emit token PIPE
                inputString++;
                break;
            case '<':

                // emit token REDIR_IN
                inputString++;
                break;
            case '>':

                if (peek(inputString) == '>') {
                    //emit token REDIR_APPEND
                    inputString += 2;
                }
                else {
                    //emit token REDIR_OUT
                    inputString++;
                }
                break;
            case '&':
                //emit token BACKGROUND
                inputString++;
                break;
            case ' ':
                inputString++;
                break;
            case '\n':  case '\0':
                return;
                break;

            default:
                currentState = IS_WORD;
                break;

            }
            break;

        case IS_WORD:

            switch (*inputString)
            {
            case '|':
                //keep the pointer where it was and change state to NORMAL
                currentState = NORMAL;
                break;
            case '<':
                //keep the pointer where it was and change state to NORMAL
                currentState = NORMAL;
                break;
            case '>':
                //keep the pointer where it was and change state to NORMAL
                currentState = NORMAL;
                break;
            case '&':
                //keep the pointer where it was and change state to NORMAL
                currentState = NORMAL;
                break;
            case '\n':  case '\0':
                //emit token word
                return;
            case ' ':
                //emit token WORD
                inputString++;

                currentState = NORMAL;
                break;

            default:
                //continue adding the character to whatever data structure we store each token as
                inputString++;
                break;
            }
            break;
        }

    }
}

//helper functions for lexer

int main() {
    char* line = NULL;
    size_t initialBufferSize = MAX_BUFFER_SIZE;
    ssize_t numofchars = 0;

    printf("$>");
    int iresult = getline(&line, &initialBufferSize, stdin);

    if (iresult == -1) {
        printf("Error");
        exit(EXIT_FAILURE);
    }

    numofchars = iresult;

    line[numofchars - 1] = '\0';
    numofchars--;

    printf("%s(number or characters = %ld)", line, numofchars);
    lexer(line);
    return 0;
}