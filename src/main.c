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
void freeTokenVector(Vector** vectorRef) {
    if (vectorRef == NULL || *vectorRef == NULL) {
        return;
    }

    Vector* vector = *vectorRef;
    Token* ptr = vec_getItemAddress(vector, 0);

    // Iteratively clean up internal heap values (e.g., strdup allocations)
    for (size_t i = 0; i < vector->length; i++) {
        delete_token(ptr);
        ptr++;
    }

    // Free the vector array buffer itself
    vec_deleteVector(vector);
    
    // Safely clear caller's pointer variable to prevent dangling usage
    *vectorRef = NULL; 
}

/**
 * Iterates through the vector and prints out a scannable token breakdown.
 */
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

int main(void) {
    char* inputBuffer = NULL;
    size_t capacity = 0;
    ssize_t charactersRead;

    while (1) {
        // 1. Shell Prompt
        printf("myshell> ");
        fflush(stdout); // Instantly flushes stdout buffer to make prompt visible

        // 2. Read Input
        charactersRead = getline(&inputBuffer, &capacity, stdin);

        // 3. Catch EOF Boundary (Ctrl+D) or Read Errors
        if (charactersRead == -1) {
            if (feof(stdin)) {
                printf("\nExiting shell cleanly.\n");
            } else {
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
        Vector* vecPtr = &tokenVector;
        freeTokenVector(&vecPtr);
    }

    // 8. Absolute Cleanup: Free the persistent line buffer created by getline
    free(inputBuffer);
    inputBuffer = NULL;

    return 0;
}