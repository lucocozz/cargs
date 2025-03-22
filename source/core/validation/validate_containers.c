#include "cargs/errors.h"
#include "cargs/internal/context.h"
#include "cargs/internal/utils.h"
#include "cargs/types.h"

int validate_subcommand(cargs_t *cargs, cargs_option_t *option)
{
    int status = CARGS_SUCCESS;

    if (option->name == NULL) {
        CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_MALFORMED_OPTION, "Subcommand must have a name");
        status = CARGS_ERROR_MALFORMED_OPTION;
    }

    if (option->sub_options == NULL) {
        CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_MALFORMED_OPTION,
                            "Subcommand '%s' must have options", option->name);
        status = CARGS_ERROR_MALFORMED_OPTION;
    }

    if (option->flags & ~SUBCOMMAND_FLAG_MASK) {
        CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_FLAG, "Invalid flags for subcommand '%s'",
                            option->name);
        status = CARGS_ERROR_INVALID_FLAG;
    }

    if (option->choices_count > 0) {
        CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_CHOICE,
                            "Subcommand '%s' cannot have choices", option->name);
        status = CARGS_ERROR_INVALID_CHOICE;
    }

    return (status);
}

int validate_group(cargs_t *cargs, cargs_option_t *option)
{
    int status = CARGS_SUCCESS;

    context_set_group(cargs, option);
    if (option->flags & ~GROUP_FLAG_MASK) {
        CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_GROUP, "Invalid flags for group '%s'",
                            option->name);
        status = CARGS_ERROR_INVALID_GROUP;
    }

    return (status);
}
