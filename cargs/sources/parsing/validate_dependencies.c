#include <string.h>
#include <stdio.h>

#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/utils.h"


static int __validate_requires(cargs_t *cargs, cargs_option_t *options, cargs_option_t *option)
{
    if (!option->requires || !option->is_set)
        return (CARGS_SUCCESS);

    for (int i = 0; option->requires[i] != NULL; ++i)
    {
        cargs_option_t *required = find_option_by_name(options, option->requires[i]);

        if (!required) {
            fprintf(stderr, "%s: Required option not found: '%s'\n", cargs->program_name, option->requires[i]);
            return (CARGS_ERROR_INVALID_DEPENDENCY);
        }
        if (!required->is_set) {
            fprintf(stderr, "%s: Required option is missing: '%s'\n", cargs->program_name, option->requires[i]);
            return (CARGS_ERROR_MISSING_REQUIRED);
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
            fprintf(stderr, "%s: Conflict: '%s' and '%s'\n", cargs->program_name, option->name, conflict->name);
            return (CARGS_ERROR_CONFLICTING_OPTIONS);
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
            cargs->active_group = option->name;
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
                fprintf(stderr, "%s: Exclusive options group '%s' conflict: '%s' and '%s'\n",
                    cargs->program_name, cargs->active_group, first_set_option_name, option->name);
                return (CARGS_ERROR_EXCLUSIVE_GROUP);
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

int validate_dependencies(cargs_t *cargs, cargs_option_t *options)
{
    int status = CARGS_SUCCESS;
    
    status = __validate_options_set(cargs, options);
    if (status != CARGS_SUCCESS)
        return (status);
    
    for (size_t i = 0; i < cargs->subcommand_depth; ++i)
    {
        const cargs_option_t *subcommand = cargs->subcommand_stack[i];
        if (!subcommand || !subcommand->subcommand.options)
            continue;

        status = __validate_options_set(cargs, subcommand->subcommand.options);
        if (status != CARGS_SUCCESS)
            return (status);
    }
    return (status);
}
