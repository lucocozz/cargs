#include <stdlib.h>
#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/internal/utils.h"


void context_init_subcommands(cargs_t *cargs)
{
    for (size_t i = 0; i < MAX_SUBCOMMAND_DEPTH; i++)
        cargs->context.subcommand_stack[i] = NULL;
    cargs->context.subcommand_depth = 0;
}

const cargs_option_t *context_get_subcommand(cargs_t *cargs)
{
    if (cargs->context.subcommand_depth == 0)
        return (NULL);
    return cargs->context.subcommand_stack[cargs->context.subcommand_depth - 1];
}

void context_push_subcommand(cargs_t *cargs, const cargs_option_t *cmd)
{
    if (cargs->context.subcommand_depth >= MAX_SUBCOMMAND_DEPTH) {
        CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_STACK_OVERFLOW, "Subcommand stack overflow");
        return;
    }
    cargs->context.subcommand_stack[cargs->context.subcommand_depth++] = cmd;
}

const cargs_option_t *context_pop_subcommand(cargs_t *cargs)
{
    if (cargs->context.subcommand_depth == 0)
        return (NULL);

    const cargs_option_t *cmd = cargs->context.subcommand_stack[--cargs->context.subcommand_depth];
    cargs->context.subcommand_stack[cargs->context.subcommand_depth] = NULL;
    return (cmd);
}

void    context_set_option(cargs_t *cargs, cargs_option_t *option) {
    cargs->context.option = option->name;
}

void    context_unset_option(cargs_t *cargs) {
    cargs->context.option = NULL;
}

void    context_set_group(cargs_t *cargs, cargs_option_t *group) {
    cargs->context.group = group->name;
}

void    context_unset_group(cargs_t *cargs) {
    cargs->context.group = NULL;
}

cargs_error_context_t get_error_context(cargs_t *cargs)
{
    const cargs_option_t *subcommand = context_get_subcommand(cargs);

    cargs_error_context_t context = {
        .option_name = cargs->context.option,
        .group_name = cargs->context.group,
        .subcommand_name = subcommand ? subcommand->name : NULL
    };
    return (context);
}

void context_init(cargs_t *cargs)
{
    cargs->context.option = NULL;
    cargs->context.group = NULL;
    context_init_subcommands(cargs);
}
