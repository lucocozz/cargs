#ifndef CARGS_UTILS_H
#define CARGS_UTILS_H

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "cargs/types.h"

#if defined(__clang__)
	#define PRAGMA_DISABLE_OVERRIDE() \
        _Pragma("clang diagnostic push") \
        _Pragma("clang diagnostic ignored \"-Winitializer-overrides\"")

	#define PRAGMA_RESTORE() \
        _Pragma("clang diagnostic pop")

#elif defined(__GNUC__)
	#define PRAGMA_DISABLE_OVERRIDE() \
        _Pragma("GCC diagnostic push") \
        _Pragma("GCC diagnostic ignored \"-Woverride-init\"")

	#define PRAGMA_RESTORE() \
        _Pragma("GCC diagnostic pop")

#else
    #define PRAGMA_DISABLE_OVERRIDE()
    #define PRAGMA_RESTORE()
#endif


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


#ifdef NO_COLOR
    #define COLOR(code, text) text
#else
    #define COLOR(code, text) code text ANSI_RESET
#endif

#define CHAR_TO_STRING(c) ((char[]){c, '\0'})
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define EXPAND(x) x
#define UNUSED(x) (void)(x)


char    *start_with(const char *prefix, const char *str);
int     cmp_value(value_type_t type, const value_t a, const value_t b);
value_t choices_to_value(value_type_t type, value_t choices, size_t choices_count, int index);

void                    subcommand_stack_init(cargs_t *cargs);
const cargs_option_t	*subcommand_current(cargs_t *cargs);
void			subcommand_push(cargs_t *cargs, const cargs_option_t *cmd);
const cargs_option_t	*subcommand_pop(cargs_t *cargs);


#endif // CARGS_UTILS_H
