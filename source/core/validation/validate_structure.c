#include "cargs/errors.h"
#include "cargs/internal/context.h"
#include "cargs/types.h"
#include <stddef.h>
#include <string.h>

int validate_subcommand(cargs_t *cargs, cargs_option_t *option);
int validate_group(cargs_t *cargs, cargs_option_t *option);
int validate_option(cargs_t *cargs, cargs_option_t *options, cargs_option_t *option);
int validate_positional(cargs_t *cargs, cargs_option_t *option);

static int ensure_validity(cargs_t *cargs, cargs_option_t *options, cargs_option_t *option)
{
    switch (option->type) {
        case TYPE_OPTION:
            return validate_option(cargs, options, option);
        case TYPE_POSITIONAL:
            return validate_positional(cargs, option);
        case TYPE_GROUP:
            return validate_group(cargs, option);
        case TYPE_SUBCOMMAND:
            return validate_subcommand(cargs, option);
        default:
            CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_MALFORMED_OPTION, "Invalid option type");
            return CARGS_ERROR_MALFORMED_OPTION;
    }
}

static int is_unique(cargs_t *cargs, cargs_option_t *option, cargs_option_t *other_option)
{
    int status = CARGS_SUCCESS;

    if (option->name && other_option->name && strcmp(option->name, other_option->name) == 0) {
        CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_DUPLICATE_OPTION, "%s: Name must be unique",
                            option->name);
        status = CARGS_ERROR_DUPLICATE_OPTION;
    }

    if (option->sname && other_option->sname && option->sname == other_option->sname) {
        CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_DUPLICATE_OPTION, "%c: Short name must be unique",
                            option->sname);
        status = CARGS_ERROR_DUPLICATE_OPTION;
    }

    if (option->lname && other_option->lname && strcmp(option->lname, other_option->lname) == 0) {
        CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_DUPLICATE_OPTION, "%s: Long name must be unique",
                            option->lname);
        status = CARGS_ERROR_DUPLICATE_OPTION;
    }

    return (status);
}

int validate_structure(cargs_t *cargs, cargs_option_t *options)
{
    bool have_helper         = false;
    bool optional_positional = false;
    int  status              = CARGS_SUCCESS;

    for (int i = 0; options[i].type != TYPE_NONE; ++i) {
        cargs_option_t *option = &options[i];
        context_set_option(cargs, option);

        int result = ensure_validity(cargs, options, option);
        if (result != CARGS_SUCCESS)
            status = result;

        for (int j = i + 1; options[j].type != TYPE_NONE; ++j) {
            cargs_option_t *other_option = &options[j];

            if (option->type != other_option->type)
                continue;

            int result = is_unique(cargs, option, other_option);
            if (result != CARGS_SUCCESS)
                status = result;
        }

        if (option->type == TYPE_OPTION && strcmp(option->name, "help") == 0)
            have_helper = true;

        // Checking bad order of positional arguments. Required positional arguments must be before
        // optional positional arguments
        if (option->type == TYPE_POSITIONAL && (option->flags & FLAG_REQUIRED) &&
            optional_positional) {
            CARGS_COLLECT_ERROR(
                cargs, CARGS_ERROR_INVALID_POSITION,
                "Required positional must be before all optional positional arguments");
            status = CARGS_ERROR_INVALID_POSITION;
        }
        if (option->type == TYPE_POSITIONAL && (option->flags & FLAG_REQUIRED) == 0)
            optional_positional = true;

        // Validate sub_options recursively
        if (option->type == TYPE_SUBCOMMAND && option->sub_options != NULL) {
            context_push_subcommand(cargs, option);
            validate_structure(cargs, option->sub_options);
            context_pop_subcommand(cargs);
        }
    }
    if (!have_helper) {
        CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_MISSING_HELP, "Missing 'help' option");
        status = CARGS_ERROR_MISSING_HELP;
    }
    return (status);
}
