#include "cargs/internal/context.h"
#include "cargs/internal/parsing.h"
#include "cargs/types.h"

int handle_subcommand(cargs_t *cargs, cargs_option_t *option, int argc, char **argv)
{
    context_push_subcommand(cargs, option);
    option->is_set = true;
    return parse_args(cargs, option->sub_options, argc, argv);
}
