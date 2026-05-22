#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "Token.h"


//the maximum buffer size in bytes 1024 for now
#define  MAX_BUFFER_SIZE 1024

char inputBuffer[MAX_BUFFER_SIZE];

typedef enum LexerState {
    NORMAL,
    IS_WORD,
    IS_OPERATOR
} LexerState;


int main() {
    char* line = NULL;
    Token tokens[MAX_BUFFER_SIZE];
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
    lexer(line, tokens);
    return 0;
}