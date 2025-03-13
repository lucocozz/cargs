/**
 * cargs/internal/utils.h - Internal utility functions
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header defines utility functions used internally by the library.
 * 
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef CARGS_INTERNAL_UTILS_H
#define CARGS_INTERNAL_UTILS_H

#include <stddef.h>
#include <stdbool.h>
#include "cargs/types.h"  /* Inclure les types publics */

/**
 * ANSI color codes for terminal output
 */
#define ANSI_RESET      "\x1b[0m"
#define ANSI_BOLD       "\x1b[1m"
#define ANSI_ITALIC     "\x1b[3m"
#define ANSI_UNDERLINE  "\x1b[4m"
#define ANSI_RED        "\x1b[31m"
#define ANSI_YELLOW     "\x1b[33m"
#define ANSI_BLUE       "\x1b[34m"
#define ANSI_GREEN      "\x1b[32m"
#define ANSI_MAGENTA    "\x1b[35m"
#define ANSI_CYAN       "\x1b[36m"
#define ANSI_WHITE      "\x1b[37m"

/**
 * Conditional color output macros
 */
#ifdef NO_COLOR
    #define COLOR(code, text) text
#else
    #define COLOR(code, text) code text ANSI_RESET
#endif

/**
 * Utility macros
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define EXPAND(x) x
#define UNUSED(x) (void)(x)

/**
 * String utility functions
 */
char *start_with(const char *prefix, const char *str);

/**
 * Value manipulation functions
 */
int     cmp_value(value_type_t type, const value_t a, const value_t b);
value_t choices_to_value(value_type_t type, value_t choices, size_t choices_count, int index);
void    free_option_value(cargs_option_t *option);

/**
 * Option lookup functions
 */
cargs_option_t *find_option_by_lname(cargs_option_t *options, const char *lname);
cargs_option_t *find_option_by_name(cargs_option_t *options, const char *name);
cargs_option_t *find_option_by_sname(cargs_option_t *options, char sname);
cargs_option_t *find_positional(cargs_option_t *options, int position);
cargs_option_t *find_subcommand(cargs_option_t *options, const char *name);
cargs_option_t *find_option_by_active_path(cargs_t cargs, const char *option_path);
const cargs_option_t *get_active_options(cargs_t *cargs);

#endif /* CARGS_INTERNAL_UTILS_H */
