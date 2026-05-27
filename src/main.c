#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Token.h"
#include "Vector.h"
#include "Lexer.h"



/**
* Iterates through the vector and prints out a scannable token breakdown.
*/



int main(void) {
    //inputbuffer to store the input from terminal. set it to null to make getline automatically alloc.
    char* inputBuffer = NULL;

    //the capacity value for the getline. set to 0 to make getline automatically allocate appropriate size.
    size_t capacity = 0;

    //variable storing the return value of the getline function to check for the success/error
    ssize_t charactersRead;

    //the main loop. Each loop: gets the line from terminal, check for errors, lexes and prints the results
    while (1) {
        // 1. Shell Prompt
        printf("$$myshell> ");

        // 2. Read Input
        charactersRead = getline(&inputBuffer, &capacity, stdin);

        // 3. Error checking for terminal stream
        if (charactersRead == -1) {
            if (feof(stdin)) {
                printf("\nExiting shell cleanly.\n");
            }
            else {
                perror("\nError reading input");
            }
            break;
        }

        // 4. Initialize Vector Container on Stack
        Vector tokenVector = vec_createVector(sizeof(Token), 8);

        // 5. Lexically Analyze Input and act directly on its status signal
        if (lexer(inputBuffer, &tokenVector)) {
            // SUCCESS CASE: Print the debug breakdown
            printVectorBuffer(&tokenVector);

            // Future step: parse_and_execute(&tokenVector);
        }
        else {
            // FAILURE CASE: The lexer already printed the syntax error message to stderr.
            // We do nothing else here, allowing execution to slide naturally into 
            // the  cleanup down below.
        }

        // 6. : No matter what happened above, the vector container 
        // was born on line 4, so it is unconditionally destroyed right here.
        freeTokenVector(&tokenVector);
    }

    // 8. Absolute Cleanup: Free the persistent line buffer created by getline
    free(inputBuffer);
    inputBuffer = NULL;

    return 0;

}

