#include <string.h>

#include "cargs/types.h"
#include "cargs/internal/parsing.h"
#include "cargs/errors.h"
#include "cargs/internal/utils.h"


int parse_args(cargs_t *cargs, cargs_option_t *options, int argc, char **argv)
{
    int     positional_index = 0;
    bool    only_positional = false;
    int     status = CARGS_SUCCESS;

    for (int i = 0; i < argc; ++i)
    {
        char *arg = argv[i];

        if (strcmp(arg, "--") == 0) {
            only_positional = true;
            continue;
        }

        if (only_positional) {
            status = handle_positional(cargs, options, arg, positional_index++);
            if (status != CARGS_SUCCESS)
                return (status);
            continue;
        }

        char *long_arg = start_with("--", arg);
        if (long_arg != NULL) {
            status = handle_long_option(cargs, options, long_arg, argv, argc, &i);
            if (status != CARGS_SUCCESS)
                return (status);
            continue;
        }

        char *short_arg = start_with("-", arg);
        if (short_arg != NULL) {
            status = handle_short_option(cargs, options, short_arg, argv, argc, &i);
            if (status != CARGS_SUCCESS)
                return (status);
            continue;
        }

        cargs_option_t *subcommand = find_subcommand(options, arg);
        if (subcommand != NULL) {
            status = handle_subcommand(cargs, subcommand, argc - i - 1, &argv[i + 1]);
            return (status);
        }

        status = handle_positional(cargs, options, arg, positional_index++);
        if (status != CARGS_SUCCESS)
            return (status);
    }
    return (status);
}
