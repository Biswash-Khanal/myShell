#ifndef LEXER_H
#define LEXER_H


#include "Vector.h"

/**
 * @brief The Lexer function. Takes the string to be lexed, and the vector where the result is to be stored.
 * 
 * @param inputString The string that is to be lexed.
 * @param vector The vector where the lexed result is to be stored.
 * @return int 0 if success.
 */
int lexer(char* inputString, Vector* vector);


#endif