#include "cargs/types.h"
#include "cargs/parsing.h"

int handle_subcommand(cargs_t *cargs, cargs_option_t *option, int argc, char **argv)
{
    const cargs_option_t *prev_subcommand = cargs->active_subcommand;

    cargs->active_subcommand = option;
    int sub_result = parse_args(cargs, option->subcommand.options, argc, argv);
    cargs->active_subcommand = prev_subcommand;

    return (sub_result);
}
