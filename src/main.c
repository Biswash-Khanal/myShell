#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "Token.h"
#include "Vector.h"
#include "Lexer.h"
#include "Parser.h"   
#include "ASTNode.h"  
#include "Executor.h"


int main(void) {
    // inputbuffer to store the input from terminal. set it to null to make getline automatically alloc.
    char* inputBuffer = NULL;

    // the capacity value for the getline. set to 0 to make getline automatically allocate appropriate size.
    size_t capacity = 0;

    // variable storing the return value of the getline function to check for the success/error
    ssize_t charactersRead;

    // the main loop. Each loop: gets the line from terminal, check for errors, lexes and prints the results
    while (1) {
        // 1. Shell Prompt
        char promptBuffer[1024];
        if (getcwd(promptBuffer, sizeof(promptBuffer)) == NULL) {
            perror("Error in getting current directory");
        }
        printf("$$myshell#%s>>", promptBuffer);
        fflush(stdout); // Forces prompt to appear immediately

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

        // 5. Lexically Analyze Input
        if (lexer(inputBuffer, &tokenVector)) {
            // OPTIONAL DEBUG: Print the raw token vector layout
            // printVectorBuffer(&tokenVector);

            // ==========================================
            // NEW PARSER INTEGRATION STEP
            // ==========================================
            // Pass tokens into the recursive descent cascade 
            ASTNode* root = parseInput(tokenVector);

            if (root != NULL) {
                printf("\n--- Generated Abstract Syntax Tree ---\n");
                print_ast_tree(root, 1);
                printf("--------------------------------------\n\n");

                // Future step: execute_ast_tree(root);
                execute_ast_tree(root);
                // Deeply clear all allocated heap structures inside this tree
                delete_ast_node(root);
                root = NULL;
            }
        }
        else {
            // FAILURE CASE: The lexer already printed the syntax error message to stderr.
        }



        // 6. Vector cleanup
        freeTokenVector(&tokenVector);


    }

    // 8. Absolute Cleanup: Free the persistent line buffer created by getline
    free(inputBuffer);
    inputBuffer = NULL;

    return 0;
}