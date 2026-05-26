#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Token.h"
#include "Vector.h"
#include "Lexer.h"

/**
 * Frees all internal token strings and deletes the vector container.
 * Expects a pointer-to-pointer so it can safely nullify the caller's reference.
 */
void freeTokenVector(Vector* tokenVector) {

    //set a pointer to the start of the buffer
    Token* ptr = vec_getItemAddress(tokenVector, 0);

    //loop through each token and call its free function
    for (size_t i = 0;i < tokenVector->length;i++) {

        delete_token(ptr);

        ptr++;
    }

    //now that individual tokens inside the buffer are freed, we can call he free vector function that frees the buffer and nulls the Vector struct
    vec_deleteVector(tokenVector);

}

/**
* Iterates through the vector and prints out a scannable token breakdown.
*/
void printVectorBuffer(const Vector* vector);


int main(void) {
    //inputbuffer to store the input from terminal. set it to null to make getline automatically alloc.
    char* inputBuffer = NULL;

    //the capacity value for the getline. set to 0 to make getline automatically allocate appropriate size.
    size_t capacity = 0;

    //variable storing the return value of the getline function to check for the success/error
    ssize_t charactersRead;

    //the main loop. Each loop: gets the line from terminal, check for errors, lexes and prints the results
    while (1) {
        // 1. Shell Prompt like $>
        printf("$$myshell> ");

        // 2. Read Input from terminal to the inputbuffer with getline
        charactersRead = getline(&inputBuffer, &capacity, stdin);

        // 3. Error checking, getline either returns the number of elements or -1 for problems
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

        // 5. Lexically Analyze Input
        lexer(inputBuffer, &tokenVector);

        // 6. Debug output
        printVectorBuffer(&tokenVector);

        // 7. Housekeeping: Free current iteration's tokens
        // We pass the address of a pointer so freeTokenVector can cleanly zero it out

        freeTokenVector(&tokenVector);
    }

    // 8. Absolute Cleanup: Free the persistent line buffer created by getline
    free(inputBuffer);
    inputBuffer = NULL;

    return 0;
    /**
     * Frees all internal token strings and deletes the vector container.
     * Expects a pointer-to-pointer so it can safely nullify the caller's reference.
     */
}


void printVectorBuffer(const Vector* vector) {
    if (vector == NULL || vector->length == 0) {
        printf("Vector Length = 0\n");
        return;
    }

    // Cast the start of the raw void* buffer to a structural Token pointer
    Token* ptr = vec_getItemAddress((Vector*)vector, 0);

    printf("Vector Length = %lu: [ ", vector->length);

    for (size_t i = 0; i < vector->length; i++) {
        // Use clean enum labels instead of raw integers (0, 1, 2...)
        switch (ptr->type) {
        case WORD:
            printf("WORD(\"%s\")", ptr->value);
            break;
        case REDIRECT_IN:
            printf("REDIRECT_IN"); // Operators don't inherently need a string value printed
            break;
        case REDIRECT_OUT:
            printf("REDIRECT_OUT");
            break;
        case REDIRECT_OUT_APPEND:
            printf("REDIRECT_OUT_APPEND");
            break;
        case PIPE:
            printf("PIPE");
            break;
        case BACKGROUND:
            printf("BACKGROUND");
            break;
        default:
            printf("UNKNOWN_TOKEN");
            break;
        }

        // Structural formatting spacer
        if (i < vector->length - 1) {
            printf(" -> ");
        }
        ptr++;
    }
    printf(" ]\n");
}