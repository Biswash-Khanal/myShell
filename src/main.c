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


    char* inputBuffer = NULL;   //string to store the input command 
    size_t capacity = 0;       //initial capacity set to 0, so that getline can alloc memory itself according to input size
    ssize_t charactersRead;     //stores the return value of the getline function in each step, to store how many characters were read

    //This is the main input output loop. The process of prompting, input capture, and then the lexing, parsing and execution happens here
    while (1) {
        //TODO comment, but basically its related to notifying background commands completion
        reap_background_zombies();

        char promptBuffer[1024];//the string to store the current prompt string, contains the shell prompt along with the current program path

        //We call the getcwd to get the current working directory, and a simple error check and error log
        if (getcwd(promptBuffer, sizeof(promptBuffer)) == NULL) {
            perror("Error in getting current directory");
        }


        //This is the shell prompt that shows up each iteration, I then concat the cwd to it so it shows the current directory and its easy to work with just like any other shells
        printf("$$myshell#%s>>", promptBuffer);
        fflush(stdout); //need to learn more, but apparently this just immediately spits out the prompt instead of saving in buffer for optimizations which sometimes can help but not very sure
        
        //System call getline, gets the input from the standard input
        charactersRead = getline(&inputBuffer, &capacity, stdin);

        //Handles cases for when getline returns -1, which can either be if end of file reached, or some error
        if (charactersRead == -1) {
            //feof function checks if the end of file condition was reached, if so we take this (CTRL+D) as a way to exit the shell cleanly
            if (feof(stdin)) {
                printf("\nExiting shell cleanly.\n");
            }
            //if it was not the end of file, something went wrong, we log the error
            else {
                perror("\nError reading input");
            }
            //if any of this condition was met, no need to continue further the loop, we exit the program by going out of the loop
            break;
        }

        //If getline was successfull, we continue forward
        //We use the custom created vector data structure and its helper functions to first create an empty vector that stores our custom Token datatype. 
        Vector tokenVector = vec_createVector(sizeof(Token), 8);

        //We then run the lexing function to capture the input buffer string and store them appropriately in the token vector. With the success return being 0, theres also a simple error check.
        if (lexer(inputBuffer, &tokenVector) == 0) {

            //if lexing was successful, then we run the parsing function on the tokenVector and store the root node to a variable
            ASTNode* root = parseInput(tokenVector);

            //If parsing was successful, we will have the root variable not be a NULL, and actually point to the root node of the AST
            if (root != NULL) {
                //debug logging to look at the generated AST structure
                printf("\n--- Generated Abstract Syntax Tree ---\n");
                print_ast_tree(root, 1);
                printf("--------------------------------------\n\n");

                //if the parsing was successful and the AST was created succesfully, then we run the execution function on the tree 
                execute_ast_tree(root, STDIN_FILENO, STDOUT_FILENO);

                //execution completes, we free up the AST
                delete_ast_node(root);
                root = NULL;
            }
        }
        else {
            
        }
        // always clear the tokenVector no matter the result in the conditional
        freeTokenVector(&tokenVector);
    }


    //We free the input buffer at the end of the program and return success
    free(inputBuffer);
    inputBuffer = NULL;

    return 0;
}