/**
 * cargs/errors.h - Error handling
 *
 * This header defines error codes and error handling functions
 * that are part of the public API.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef CARGS_ERRORS_H
#define CARGS_ERRORS_H

#include "cargs/types.h"
#include <stddef.h>

/**
 * Error codes returned by cargs functions
 */
typedef enum cargs_error_type_e
{
    CARGS_SUCCESS = 0,
    CARGS_SOULD_EXIT,

    /* Structure errors */
    CARGS_ERROR_DUPLICATE_OPTION,
    CARGS_ERROR_INVALID_HANDLER,
    CARGS_ERROR_INVALID_DEFAULT,
    CARGS_ERROR_INVALID_GROUP,
    CARGS_ERROR_INVALID_DEPENDENCY,
    CARGS_ERROR_INVALID_FLAG,
    CARGS_ERROR_INVALID_POSITION,
    CARGS_ERROR_MALFORMED_OPTION,
    CARGS_ERROR_MISSING_HELP,

    /* Parsing errors */
    CARGS_ERROR_INVALID_ARGUMENT,
    CARGS_ERROR_MISSING_VALUE,
    CARGS_ERROR_MISSING_REQUIRED,
    CARGS_ERROR_CONFLICTING_OPTIONS,
    CARGS_ERROR_INVALID_FORMAT,
    CARGS_ERROR_EXCLUSIVE_GROUP,
    CARGS_ERROR_INVALID_CHOICE,
    CARGS_ERROR_INVALID_RANGE,

    /* Execution errors */
    CARGS_ERROR_NO_COMMAND,

    /* Internal errors */
    CARGS_ERROR_MEMORY,

    /* Value errors */
    CARGS_ERROR_INVALID_VALUE,

    /* Stack errors */
    CARGS_ERROR_STACK_OVERFLOW,
} cargs_error_type_t;

/**
 * cargs_print_error_stack - Print all errors in the error stack
 *
 * @param cargs  Cargs context
 */
void cargs_print_error_stack(const cargs_t *cargs);

/**
 * cargs_strerror - Get string description of an error code
 *
 * @param error  Error code
 * @return String description of the error
 */
const char *cargs_strerror(cargs_error_type_t error);

/* Forward declaration of internal function */
cargs_error_context_t get_error_context(cargs_t *cargs);
void                  cargs_push_error(cargs_t *cargs, cargs_error_t error);

#include <stdarg.h>
#include <stdio.h>

#define CARGS_OK() ((cargs_error_t){.code = CARGS_SUCCESS})

/* Functions that implement the actual error handling logic */
static inline void cargs_collect_error(cargs_t *cargs, int code, const char *fmt, ...)
{
    cargs_error_t error;
    va_list       args;

    error.code    = code;
    error.context = get_error_context(cargs);

    va_start(args, fmt);
    vsnprintf(error.message, CARGS_MAX_ERROR_MESSAGE_SIZE, fmt, args);
    va_end(args);

    cargs_push_error(cargs, error);
}

static inline int cargs_report_error(cargs_t *cargs, int code, const char *fmt, ...)
{
    va_list args;

    fprintf(stderr, "%s: ", cargs->program_name);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
    cargs->error_stack.count++;
    return code;
}

/**
 * CARGS_COLLECT_ERROR - Collect an error in the error stack
 * This version uses an inline function to handle the variadic arguments correctly
 */
#define CARGS_COLLECT_ERROR(cargs, code, ...) cargs_collect_error(cargs, code, __VA_ARGS__)

/**
 * CARGS_REPORT_ERROR - Report an error and return
 * This version uses an inline function to handle the variadic arguments correctly
 */
#define CARGS_REPORT_ERROR(cargs, code, ...) return cargs_report_error(cargs, code, __VA_ARGS__)

#endif /* CARGS_ERRORS_H */
