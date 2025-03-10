#ifndef CARGS_ERRORS_H
#define CARGS_ERRORS_H

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>


typedef struct cargs_s cargs_t;
typedef struct cargs_option_s cargs_option_t;

#ifndef CARGS_MAX_ERRORS_STACK
# define CARGS_MAX_ERRORS_STACK 16
#endif
#ifndef CARGS_MAX_ERROR_MESSAGE_SIZE
# define CARGS_MAX_ERROR_MESSAGE_SIZE 256
#endif

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

    // Value errors
    CARGS_ERROR_INVALID_VALUE,

    // Stack errors
    CARGS_ERROR_STACK_OVERFLOW,
} cargs_error_type_t;


typedef struct cargs_error_context_s {
    const char *option_name;
    const char *group_name;
    const char *subcommand_name;
} cargs_error_context_t;

typedef struct cargs_error_s {
    cargs_error_context_t   context;
    cargs_error_type_t      code;
    char                    message[CARGS_MAX_ERROR_MESSAGE_SIZE];
} cargs_error_t;

typedef struct cargs_error_stack_s {
    cargs_error_t   errors[CARGS_MAX_ERRORS_STACK];
    size_t          count;
} cargs_error_stack_t;


#define CARGS_COLLECT_ERROR(cargs, _code, msg, ...) do { \
    cargs_error_t error; \
    error.code = _code; \
    error.context = get_error_context(cargs); \
    snprintf(error.message, CARGS_MAX_ERROR_MESSAGE_SIZE, msg, ##__VA_ARGS__); \
    cargs_push_error(cargs, error); \
} while (0)

#define CARGS_REPORT_ERROR(cargs, code, msg, ...) do { \
    fprintf(stderr, "%s: " msg "\n", cargs->program_name, ##__VA_ARGS__); \
    return (code); \
} while (0)

#define CARGS_OK() (cargs_error_t) { .code = CARGS_SUCCESS }


// API Functions declarations
void        cargs_print_error_stack(const cargs_t *cargs);
const char  *cargs_strerror(cargs_error_type_t error);
void        cargs_push_error(cargs_t *cargs, cargs_error_t error);
void        cargs_clear_errors(cargs_t *cargs);

#endif // CARGS_ERRORS_H
