#include "Vector.h"
#include "Token.h"

static Token* get_token_pointer(Vector tokenVector, size_t* cursor) {
    if (*cursor <= 0 || *cursor >= tokenVector.length) {
        return NULL;
    }
    return ((Token*)vec_getItemAddress(&tokenVector, *cursor));
}

void parseInput(Vector tokenVector) {
    //the cursor used for the parsing continuation between the recursive functions
    size_t cursor = 0;

    //for now lets just call the function
    parse_background(tokenVector, &cursor);
}


void parse_background(Vector tokenVector, size_t* cursor) {
    parse_pipeline(tokenVector, cursor);
}

void parse_pipeline(Vector tokenVector, size_t* cursor) {
    parse_redirection(tokenVector, cursor);
}
void parse_redirection(Vector tokenVector, size_t* cursor) {
    parse_commands(tokenVector, cursor);
}
void parse_commands(Vector tokenVector, size_t* cursor) {

    if (tokenVector.length == 0 || cursor == NULL) {
        //if maybe somethowe cursor pointer is null
        //exit program for now TODO:appropriate error handling in case empty tokenVector is recieved
        exit(EXIT_FAILURE);
    }

    if (get_token_pointer(tokenVector, cursor)->type != WORD) {
        return;
    }




    return;
}