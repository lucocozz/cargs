#include <stdlib.h>

#include "cargs/internal/utils.h"
#include "cargs/types.h"

static void free_options(cargs_option_t *options)
{
    for (cargs_option_t *option = options; option->type != TYPE_NONE; ++option)
        free_option_value(option);
}

void cargs_free(cargs_t *cargs)
{
    free_options(cargs->options);
    for (size_t i = 0; i < cargs->context.subcommand_depth; ++i) {
        const cargs_option_t *subcommand = cargs->context.subcommand_stack[i];
        cargs_option_t       *options    = subcommand->sub_options;
        free_options(options);
    }
}
