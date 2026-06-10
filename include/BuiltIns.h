#ifndef BUILTINS_H
#define BUILTINS_H

#include <stddef.h> 


typedef int (*BuiltInHandler)(char** args);

typedef struct {
    const char* name;
    BuiltInHandler handler;
} BuiltInRegistry;


int shell_cd(char** args);
int shell_pwd(char** args);
int shell_cat(char** args);


extern const BuiltInRegistry builtin_table[];
extern const size_t builtin_count;

#endif 