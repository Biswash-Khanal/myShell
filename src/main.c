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

    char* inputBuffer = NULL;
    size_t capacity = 0;
    ssize_t charactersRead;

    while (1) {
        reap_background_zombies();

        char promptBuffer[1024];


        if (getcwd(promptBuffer, sizeof(promptBuffer)) == NULL) {
            perror("Error in getting current directory");
        }


        printf("$$myshell#%s>>", promptBuffer);
        fflush(stdout);


        charactersRead = getline(&inputBuffer, &capacity, stdin);


        if (charactersRead == -1) {
            if (feof(stdin)) {
                printf("\nExiting shell cleanly.\n");
            }
            else {
                perror("\nError reading input");
            }
            break;
        }


        Vector tokenVector = vec_createVector(sizeof(Token), 8);


        if (lexer(inputBuffer, &tokenVector) == 0) {

            ASTNode* root = parseInput(tokenVector);


            if (root != NULL) {
                printf("\n--- Generated Abstract Syntax Tree ---\n");
                print_ast_tree(root, 1);
                printf("--------------------------------------\n\n");

                execute_ast_tree(root, STDIN_FILENO, STDOUT_FILENO);
                delete_ast_node(root);
                root = NULL;
            }
        }
        else {
            
        }




        freeTokenVector(&tokenVector);


    }


    free(inputBuffer);
    inputBuffer = NULL;

    return 0;
}