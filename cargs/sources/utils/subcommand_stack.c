#include <stdlib.h>
#include "cargs/types.h"
#include "cargs/errors.h"

void subcommand_stack_init(cargs_t *cargs)
{
    for (size_t i = 0; i < MAX_SUBCOMMAND_DEPTH; i++) {
        cargs->subcommand_stack[i] = NULL;
    }
    cargs->subcommand_depth = 0;
}

const cargs_option_t *subcommand_current(cargs_t *cargs)
{
    if (cargs->subcommand_depth == 0)
        return NULL;
    return cargs->subcommand_stack[cargs->subcommand_depth - 1];
}

void subcommand_push(cargs_t *cargs, const cargs_option_t *cmd)
{
    if (cargs->subcommand_depth >= MAX_SUBCOMMAND_DEPTH) {
        cargs_error_t error = CARGS_ERROR(
            CARGS_ERROR_STACK_OVERFLOW, 
            "Maximum subcommand depth reached",
            CARGS_ERROR_CONTEXT(cargs, cmd)
        );
        cargs_push_error(cargs, error);
        return;
    }
    
    cargs->subcommand_stack[cargs->subcommand_depth++] = cmd;
}

const cargs_option_t *subcommand_pop(cargs_t *cargs)
{
    if (cargs->subcommand_depth == 0)
        return NULL;
        
    const cargs_option_t *cmd = cargs->subcommand_stack[--cargs->subcommand_depth];
    cargs->subcommand_stack[cargs->subcommand_depth] = NULL;
    return cmd;
}
