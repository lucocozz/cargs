#include <string.h>
#include <stdio.h>

#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/internal/context.h"
#include "cargs/internal/utils.h"


static int __validate_required_positional(cargs_t *cargs, cargs_option_t *options)
{
    for (int i = 0; options[i].type != TYPE_NONE; ++i)
    {
        cargs_option_t *option = &options[i];
        
        if (option->type == TYPE_POSITIONAL && (option->flags & FLAG_REQUIRED) && !option->is_set) {
            CARGS_REPORT_ERROR(cargs, CARGS_ERROR_MISSING_REQUIRED,
                "Required positional argument missing: '%s'", option->name);
        }
    }
    return (CARGS_SUCCESS);
}

static int __validate_requires(cargs_t *cargs, cargs_option_t *options, cargs_option_t *option)
{
    if (!option->requires || !option->is_set)
        return (CARGS_SUCCESS);

    for (int i = 0; option->requires[i] != NULL; ++i)
    {
        cargs_option_t *required = find_option_by_name(options, option->requires[i]);

        if (!required) {
            CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_DEPENDENCY,
                "Required option not found '%s' for option '%s'", option->requires[i], option->name);
        }
        if (!required->is_set) {
            CARGS_REPORT_ERROR(cargs, CARGS_ERROR_MISSING_REQUIRED,
                "Required option is missing: '%s' with option '%s'", option->requires[i], option->name);
        }
    }
    return (CARGS_SUCCESS);
}

static int __validate_conflicts(cargs_t *cargs, cargs_option_t *options, cargs_option_t *option)
{
    if (!option->conflicts || !option->is_set)
        return (CARGS_SUCCESS);

    for (int i = 0; option->conflicts[i] != NULL; ++i)
    {
        cargs_option_t *conflict = find_option_by_name(options, option->conflicts[i]);
        if (conflict && conflict->is_set) {
            CARGS_REPORT_ERROR(cargs, CARGS_ERROR_CONFLICTING_OPTIONS,
                "Conflict between '%s' and '%s'", option->name, conflict->name);
        }
    }
    return (CARGS_SUCCESS);
}

static int __validate_exclusive_groups(cargs_t *cargs, cargs_option_t *options)
{
    bool        current_group_is_exclusive = false;
    const char  *first_set_option_name = NULL;

    for (int i = 0; options[i].type != TYPE_NONE; ++i)
    {
        cargs_option_t *option = &options[i];

        if (option->type == TYPE_GROUP)
        {
            context_set_group(cargs, option);
            current_group_is_exclusive = option->flags & FLAG_EXCLUSIVE;
            first_set_option_name = NULL;
            continue;
        }

        if (!option->is_set)
            continue;

        if (current_group_is_exclusive)
        {
            if (!first_set_option_name)
                first_set_option_name = option->name;
            else {
                CARGS_REPORT_ERROR(cargs, CARGS_ERROR_EXCLUSIVE_GROUP,
                    "Exclusive options group '%s' conflict: '%s' and '%s'",
                    cargs->context.group, first_set_option_name, option->name);
            }
        }
    }
    return (CARGS_SUCCESS);
}

static int __validate_options_set(cargs_t *cargs, cargs_option_t *options)
{
    int status = CARGS_SUCCESS;
    
    status = __validate_exclusive_groups(cargs, options);
    if (status != CARGS_SUCCESS)
        return (status);

    status = __validate_required_positional(cargs, options);
    if (status != CARGS_SUCCESS)
        return (status);

    for (int i = 0; options[i].type != TYPE_NONE; ++i)
    {
        cargs_option_t *option = &options[i];
        
        if (option->type == TYPE_GROUP || !option->is_set)
            continue;

        status = __validate_requires(cargs, options, option);
        if (status != CARGS_SUCCESS)
            return (status);

        status = __validate_conflicts(cargs, options, option);
        if (status != CARGS_SUCCESS)
            return (status);
    }
    return (status);
}

int post_parse_validation(cargs_t *cargs, cargs_option_t *options)
{
    int status = CARGS_SUCCESS;
    
    status = __validate_options_set(cargs, options);
    if (status != CARGS_SUCCESS)
        return (status);
    
    for (size_t i = 0; i < cargs->context.subcommand_depth; ++i)
    {
        const cargs_option_t *subcommand = cargs->context.subcommand_stack[i];
        if (!subcommand || !subcommand->sub_options)
            continue;

        status = __validate_options_set(cargs, subcommand->sub_options);
        if (status != CARGS_SUCCESS)
            return (status);
    }
    return (status);
}
