#ifndef BUILTINS_H
#define BUILTINS_H

#include <stddef.h> // Needed for size_t

// 1. Blueprints
typedef int (*BuiltInHandler)(char** args);

typedef struct {
    const char* name;
    BuiltInHandler handler;
} BuiltInRegistry;

// 2. Standalone function signatures
int shell_cd(char** args);
int shell_pwd(char** args);
int shell_cat(char** args);

// 3. EXTERN PROMISES: Tell other files these exist without defining them here
extern const BuiltInRegistry builtin_table[];
extern const size_t builtin_count;

#endif // BUILTINS_H