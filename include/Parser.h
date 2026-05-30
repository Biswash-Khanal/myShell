#include "ASTNode.h"
#include "Vector.h"

//the entry point function
/**
 *@brief Parses the token Vector into the Abstract Syntax Tree
 *
 * @param tokenVector The vector to be parsed.
 *
 */
void parseInput(Vector tokenVector);


//the 4 worker functions

/**
 *@brief Worker function that calls the other worker recursively. This function is responsible for properly parsing the background operator logics
 *
 * @param tokenVector The token Vector
 * @param cursor  The pointer to the cursor integer from the entry point functoin
 */
void parse_background(Vector tokenVector, int* cursor);


/**
 *@brief Worker function that calls the other worker recursively. This function is responsible for properly parsing the pipeline operator logics
 *
 * @param tokenVector The token Vector
 * @param cursor  The pointer to the cursor integer from the entry point functoin
 */
void parse_pipeline(Vector tokenVector, int* cursor);

/**
 *@brief Worker function that calls the other worker recursively. This function is responsible for properly parsing the Redicrection operators logics
 *
 * @param tokenVector The token Vector
 * @param cursor  The pointer to the cursor integer from the entry point functoin
 */
void parse_redirection(Vector tokenVector, int* cursor);

/**
 *@brief The leaf parsing function. All other worker calls reaches here once. Responsivble for parsing the commands. returns the control back if not a word.
 *
 * @param tokenVector The token Vector
 * @param cursor  The pointer to the cursor integer from the entry point functoin
 */
void parse_commands(Vector tokenVector, int* cursor);