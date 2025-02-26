#ifndef CARGS_ERRORS_H
#define CARGS_ERRORS_H


#include <stddef.h>

#include "cargs/types.h"

#define CARGS_MAX_ERRORS_STACK 16


typedef enum cargs_error_type_e {
	CARGS_SUCCESS = 0,

    CARGS_ERROR_DUPLICATE_OPTION,
    CARGS_ERROR_INVALID_HANDLER,
    // CARGS_ERROR_INVALID_VALIDATOR,
    CARGS_ERROR_INVALID_DEFAULT,
    CARGS_ERROR_INVALID_GROUP,
    CARGS_ERROR_INVALID_DEPENDENCY,
    CARGS_ERROR_INVALID_FLAG,
    CARGS_ERROR_MALFORMED_OPTION,

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


#define CARGS_PUSH_ERROR(cargs, error, message, context) \
    cargs_push_error(cargs, error, message, (cargs_error_context_t)context)

#define CARGS_PUSH_ERROR_INFO(cargs, error_info) \
    cargs_push_error(cargs, error_info.code, error_info.message, error_info.context)


#define CARGS_ERROR(_code, _message, _context) \
    (cargs_error_t) { .code = _code, .message = _message, .context = _context }

#define CARGS_ERROR_CONTEXT(cargs, option) \
    (cargs_error_context_t) { .option_name = option->name, .group_name = cargs->active_group, .subcommand_name = cargs->active_subcommand }

typedef struct cargs_s cargs_t;

void        cargs_print_error_stack(const cargs_t *cargs);
const char  *cargs_strerror(cargs_error_type_t error);
void        cargs_push_error(cargs_t *cargs, cargs_error_type_t error, const char *message, cargs_error_context_t context);
void        cargs_clear_errors(cargs_t *cargs);


#endif // CARGS_ERRORS_H
