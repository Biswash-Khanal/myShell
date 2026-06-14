#ifndef BUILTINS_H
#define BUILTINS_H

#include <stddef.h> 

/**
 * @brief Typedef for a function pointer representing a built-in command handler.
 *
 * Each built-in handler function takes an array of string arguments (char** args)
 * and returns an integer status code. This allows us to store and call handlers
 * uniformly without knowing their specific implementation details.
 */
typedef int (*BuiltInHandler)(char** args);

/**
 * @brief Structure representing a single built-in command entry.
 *
 * Contains:
 * - name: the string name of the built-in command (e.g., "cd", "exit").
 * - handler: the function pointer to the handler implementation.
 *
 * This structure is used to build a registry table of all built-in commands
 * so that we can loop through and match user input against available built-ins.
 */
typedef struct {
    const char* name;
    BuiltInHandler handler;
} BuiltInRegistry;

/**
 * @brief Built-in command implementations.
 *
 * Each of these functions matches a shell built-in and provides its logic:
 * - shell_cd: change directory.
 * - shell_pwd: print working directory.
 * - shell_cat: concatenate and print file contents.
 *
 * They all follow the BuiltInHandler signature so they can be stored in the registry.
 */
int shell_cd(char** args);
int shell_pwd(char** args);
int shell_cat(char** args);

/**
 * @brief External references to the built-in registry table and its size.
 *
 * - builtin_table: array of BuiltInRegistry entries defined elsewhere (likely in BuiltIns.c).
 * - builtin_count: number of entries in builtin_table.
 *
 * These are declared extern here so that other modules (like Executor) can access
 * the registry without duplicating its definition.
 */
extern const BuiltInRegistry builtin_table[];
extern const size_t builtin_count;

#endif
