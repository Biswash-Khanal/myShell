#ifndef LEXER_H
#define LEXER_H
#include "Vector.h"


/**
 * @brief The main Lexer function, goes character by character creating tokens
 *
 * @param inputString The input string provided by the user
 * @param vector The vector where each tokens will be stored at
 */
void lexer(char* inputString, Vector* vector);

/**
 * @brief returns 1 if character is an operator, 0 if not
 *
 * @param inputCharacter
 * @return int
 */
int isOperator(char inputCharacter);

/**
 * @brief returns 1 if the character is a word compatible character, 0 if not
 *
 * @param inputCharacter
 * @return int
 */
int isCharacter(char inputCharacter);

#endif