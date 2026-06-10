#ifndef PARSER_H
#define PARSER_H


#include "ASTNode.h"
#include "Vector.h"


ASTNode* parseInput(Vector tokenVector);



ASTNode* parse_commands(Vector tokenVector, size_t* cursor);


ASTNode* parse_redirection(Vector tokenVector, size_t* cursor);


ASTNode* parse_pipeline(Vector tokenVector, size_t* cursor);


ASTNode* parse_background(Vector tokenVector, size_t* cursor);


#endif