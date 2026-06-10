#ifndef LEXER_H
#define LEXER_H


#include "Vector.h"



int lexer(char* inputString, Vector* vector);


int isOperator(char inputCharacter);


int isCharacter(char inputCharacter);

#endif