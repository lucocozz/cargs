#include <stdio.h>
#include <string.h>

#include "cargs/errors.h"
#include "cargs/internal/context.h"
#include "cargs/internal/utils.h"
#include "cargs/types.h"

static int validate_choices(cargs_t *cargs, cargs_option_t *option)
{
    if (option->choices_count > 0) {
        bool valid_choices = false;
        for (size_t i = 0; i < option->choices_count && !valid_choices; ++i) {
            cargs_value_t choice =
                choices_to_value(option->value_type, option->choices, option->choices_count, i);
            valid_choices = (cmp_value(option->value_type, option->value, choice) == 0);
        }
        if (!valid_choices) {
            fprintf(stderr, "%s: The '%s' option cannot be set to '", cargs->program_name,
                    option->name);
            print_value(stderr, option->value_type, option->value);
            fprintf(stderr, "'. Please choose from ");
            print_value_array(stderr, option->value_type, option->choices.as_ptr,
                              option->choices_count);
            fprintf(stderr, "\n");
            return (CARGS_ERROR_INVALID_CHOICE);
        }
    }
    return (CARGS_SUCCESS);
}

static int validate_required(cargs_t *cargs, cargs_option_t *options, cargs_option_t *option)
{
    if (option->requires) {
        for (int j = 0; option->requires[j] != NULL; ++j) {
            cargs_option_t *required = find_option_by_name(options, option->requires[j]);
            if (required && !required->is_set) {
                CARGS_REPORT_ERROR(cargs, CARGS_ERROR_MISSING_REQUIRED,
                                   "Required option is missing: '%s' with option '%s'",
                                   option->requires[j], option->name);
            }
        }
    }
    return (CARGS_SUCCESS);
}

static int validate_conflicts(cargs_t *cargs, cargs_option_t *options, cargs_option_t *option)
{
    if (option->conflicts) {
        for (int j = 0; option->conflicts[j] != NULL; ++j) {
            cargs_option_t *conflict = find_option_by_name(options, option->conflicts[j]);
            if (conflict && conflict->is_set) {
                CARGS_REPORT_ERROR(cargs, CARGS_ERROR_CONFLICTING_OPTIONS,
                                   "Conflict between '%s' and '%s'", option->name, conflict->name);
            }
        }
    }
    return (CARGS_SUCCESS);
}

static int call_validators(cargs_t *cargs, cargs_option_t *option)
{
    for (size_t i = 0; i < option->validator_count; ++i) {
        validator_entry_t *validator = &option->validators[i];
        if (validator->func == NULL)
            continue;
        int status = validator->func(cargs, option, validator->data);
        if (status != CARGS_SUCCESS)
            return (status);
    }
    return (CARGS_SUCCESS);
}

static int validate_options_set(cargs_t *cargs, cargs_option_t *options)
{
    bool        current_group_is_exclusive = false;
    const char *first_set_option_name      = NULL;

    for (int i = 0; options[i].type != TYPE_NONE; ++i) {
        cargs_option_t *option = &options[i];

        if (option->type == TYPE_GROUP) {
            context_set_group(cargs, option);
            current_group_is_exclusive = option->flags & FLAG_EXCLUSIVE;
            first_set_option_name      = NULL;
            continue;
        }

        if (option->type == TYPE_POSITIONAL && (option->flags & FLAG_REQUIRED) && !option->is_set) {
            CARGS_REPORT_ERROR(cargs, CARGS_ERROR_MISSING_REQUIRED,
                               "Required positional argument missing: '%s'", option->name);
        }

        if (option->is_set) {
            int status;

            if (current_group_is_exclusive) {
                if (first_set_option_name == NULL) {
                    first_set_option_name = option->name;
                } else {
                    CARGS_REPORT_ERROR(cargs, CARGS_ERROR_EXCLUSIVE_GROUP,
                                       "Exclusive options group '%s' conflict: '%s' and '%s'",
                                       cargs->context.group, first_set_option_name, option->name);
                }
            }

            status = call_validators(cargs, option);
            if (status != CARGS_SUCCESS)
                return (status);

            status = validate_choices(cargs, option);
            if (status != CARGS_SUCCESS)
                return (status);

            status = validate_required(cargs, options, option);
            if (status != CARGS_SUCCESS)
                return (status);

            status = validate_conflicts(cargs, options, option);
            if (status != CARGS_SUCCESS)
                return (status);
        }
    }
    return (CARGS_SUCCESS);
}

int post_parse_validation(cargs_t *cargs)
{
    int status;

    status = validate_options_set(cargs, cargs->options);
    if (status != CARGS_SUCCESS)
        return status;

    for (size_t i = 0; i < cargs->context.subcommand_depth; ++i) {
        const cargs_option_t *subcommand = cargs->context.subcommand_stack[i];
        if (!subcommand || !subcommand->sub_options)
            continue;

        int subcommand_status = validate_options_set(cargs, subcommand->sub_options);
        if (subcommand_status != CARGS_SUCCESS)
            return subcommand_status;
    }

    return status;
}
