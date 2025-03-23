#include "cargs/errors.h"
#include "cargs/internal/utils.h"
#include "cargs/types.h"

#include <stdio.h>

const char *cargs_strerror(cargs_error_type_t error)
{
    switch (error) {
        case CARGS_SUCCESS:
            return "Success";
        case CARGS_ERROR_DUPLICATE_OPTION:
            return "Duplicate option";
        case CARGS_ERROR_INVALID_HANDLER:
            return "Invalid handler";
        case CARGS_ERROR_INVALID_DEFAULT:
            return "Invalid default value";
        case CARGS_ERROR_INVALID_GROUP:
            return "Invalid group";
        case CARGS_ERROR_INVALID_DEPENDENCY:
            return "Invalid dependency";
        case CARGS_ERROR_INVALID_FLAG:
            return "Invalid flag";
        case CARGS_ERROR_INVALID_POSITION:
            return "Invalid position";
        case CARGS_ERROR_INVALID_ARGUMENT:
            return "Invalid argument";
        case CARGS_ERROR_MISSING_VALUE:
            return "Missing value";
        case CARGS_ERROR_MISSING_REQUIRED:
            return "Missing required option";
        case CARGS_ERROR_CONFLICTING_OPTIONS:
            return "Conflicting options";
        case CARGS_ERROR_INVALID_FORMAT:
            return "Invalid format";
        case CARGS_ERROR_EXCLUSIVE_GROUP:
            return "Exclusive group";
        case CARGS_ERROR_INVALID_CHOICE:
            return "Invalid choice";
        case CARGS_ERROR_INVALID_RANGE:
            return "Invalid range";
        case CARGS_ERROR_NO_COMMAND:
            return "No command";
        case CARGS_ERROR_INVALID_VALUE:
            return "Invalid value";
        case CARGS_ERROR_MALFORMED_OPTION:
            return "Malformed option";
        case CARGS_ERROR_MISSING_HELP:
            return "Missing help option";
        case CARGS_ERROR_STACK_OVERFLOW:
            return "Error stack overflow";
        default:
            return "Unknown error";
    }
}

void cargs_print_error_stack(const cargs_t *cargs)
{
    if (cargs->error_stack.count == 0)
        return;

    fprintf(stderr, COLOR(ANSI_BOLD ANSI_RED, "Error stack (%zu errors):\n"),
            cargs->error_stack.count);

    for (size_t i = 0; i < cargs->error_stack.count && i < CARGS_MAX_ERRORS_STACK; ++i) {
        const cargs_error_t *error = &cargs->error_stack.errors[i];

        fprintf(stderr, COLOR(ANSI_BOLD, "[%zu]"), i + 1);

        if (error->context.subcommand_name) {
            fprintf(stderr, " in subcommand ");
            fprintf(stderr, COLOR(ANSI_GREEN ANSI_BOLD, "'%s'"), error->context.subcommand_name);
        }

        if (error->context.group_name) {
            fprintf(stderr, " in group ");
            fprintf(stderr, COLOR(ANSI_MAGENTA ANSI_BOLD, "'%s'"), error->context.group_name);
        }

        if (error->context.option_name) {
            fprintf(stderr, " for option ");
            fprintf(stderr, COLOR(ANSI_CYAN ANSI_BOLD, "'%s'"), error->context.option_name);
        }
        fprintf(stderr, "\n");

        fprintf(stderr, COLOR(ANSI_BOLD, "\tError: "));
        fprintf(stderr, COLOR(ANSI_YELLOW ANSI_BOLD, "%s\n"), cargs_strerror(error->code));

        if (error->message[0] != '\0') {
            fprintf(stderr, COLOR(ANSI_BOLD, "\tDetails: "));
            fprintf(stderr, COLOR(ANSI_ITALIC, "%s\n"), error->message);
        }
    }
    if (cargs->error_stack.count >= CARGS_MAX_ERRORS_STACK) {
        printf(COLOR(ANSI_ITALIC ANSI_RED, "(Too many errors, only the first %d are displayed)\n"),
               CARGS_MAX_ERRORS_STACK);
    }
}

void cargs_clear_errors(cargs_t *cargs)
{
    cargs->error_stack.count = 0;
}

void cargs_push_error(cargs_t *cargs, cargs_error_t error)
{
    if (cargs->error_stack.count >= CARGS_MAX_ERRORS_STACK) {
        cargs_error_t *last = &cargs->error_stack.errors[CARGS_MAX_ERRORS_STACK - 1];
        last->context       = (cargs_error_context_t){0};
        last->code          = CARGS_ERROR_STACK_OVERFLOW;
        snprintf(last->message, CARGS_MAX_ERROR_MESSAGE_SIZE, "Too many errors");
        return;
    }

    cargs_error_t *last = &cargs->error_stack.errors[cargs->error_stack.count++];
    *last               = error;
}
