#include "cargs/types.h"
#include "cargs/internal/parsing.h"
#include "cargs/internal/context.h"
#include "cargs/internal/utils.h"


int handle_subcommand(cargs_t *cargs, cargs_option_t *option, int argc, char **argv)
{
    context_push_subcommand(cargs, option);
    return parse_args(cargs, option->subcommand.options, argc, argv);
}
