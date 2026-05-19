#ifndef GUARDS_H
#define GUARDS_H

/**
 *@brief An out of memory guard wrapper function.
 * 
 * @param ptr The pointer to be checked
 * @param file File name where the fault occured
 * @param number Line number where the fault occured
 */
void OOM_GUARD(void *ptr, char* file, int number);

#endif