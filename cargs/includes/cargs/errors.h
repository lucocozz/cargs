#ifndef CARGS_ERRORS_H
#define CARGS_ERRORS_H

#include <stddef.h>

typedef struct cargs_s cargs_t;
typedef struct cargs_option_s cargs_option_t;

#define CARGS_MAX_ERRORS_STACK 16

typedef enum cargs_error_type_e {
	CARGS_SUCCESS = 0,

    // Structure errors
    CARGS_ERROR_DUPLICATE_OPTION,
    CARGS_ERROR_INVALID_HANDLER,
    CARGS_ERROR_INVALID_DEFAULT,
    CARGS_ERROR_INVALID_GROUP,
    CARGS_ERROR_INVALID_DEPENDENCY,
    CARGS_ERROR_INVALID_FLAG,
    CARGS_ERROR_MALFORMED_OPTION,

    // Parsing errors
    CARGS_ERROR_INVALID_ARGUMENT,
    CARGS_ERROR_MISSING_VALUE,
    CARGS_ERROR_MISSING_REQUIRED,
    CARGS_ERROR_CONFLICTING_OPTIONS,
    CARGS_ERROR_INVALID_FORMAT,
    CARGS_ERROR_EXCLUSIVE_GROUP,
    CARGS_ERROR_INVALID_CHOICE,
    CARGS_ERROR_INVALID_RANGE,
    
    // Stack errors
    CARGS_ERROR_STACK_OVERFLOW,
} cargs_error_type_t;


typedef struct cargs_error_context_s {
    const char *option_name;
    const char *group_name;
    const char *subcommand_name;
} cargs_error_context_t;

typedef struct cargs_error_s {
    cargs_error_type_t      code;
    const char              *message;
    cargs_error_context_t   context;
} cargs_error_t;

typedef struct cargs_error_stack_s {
    cargs_error_t   errors[CARGS_MAX_ERRORS_STACK];
    size_t          count;
} cargs_error_stack_t;



#define CARGS_ERROR(_code, _message, _context) \
(cargs_error_t) { .code = _code, .message = _message, .context = _context }

#define CARGS_ERROR_CONTEXT(cargs, option) (cargs_error_context_t) { \
    .option_name = option->name, \
    .group_name = cargs->active_group, \
    .subcommand_name = subcommand_current(cargs) ? subcommand_current(cargs)->name : NULL \
}

#define CARGS_ERROR_DEFAULT(cargs) \
    (cargs_error_t) { .code = CARGS_SUCCESS, .message = NULL, .context = { \
        .option_name = NULL, .group_name = cargs->active_group , \
        .subcommand_name = subcommand_current(cargs) ? subcommand_current(cargs)->name : NULL \
}}


void        cargs_print_error_stack(const cargs_t *cargs);
const char  *cargs_strerror(cargs_error_type_t error);
void        cargs_push_error(cargs_t *cargs, cargs_error_t error);
void        cargs_clear_errors(cargs_t *cargs);


#endif // CARGS_ERRORS_H
