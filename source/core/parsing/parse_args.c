#include <ctype.h>
#include <string.h>

#include "cargs/errors.h"
#include "cargs/internal/parsing.h"
#include "cargs/internal/utils.h"
#include "cargs/types.h"

int parse_args(cargs_t *cargs, cargs_option_t *options, int argc, char **argv)
{
    int  positional_index = 0;
    bool only_positional  = false;
    int  status           = CARGS_SUCCESS;

    for (int i = 0; i < argc; ++i) {
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

        char *long_arg = starts_with("--", arg);
        if (long_arg != NULL) {
            status = handle_long_option(cargs, options, long_arg, argv, argc, &i);
            if (status != CARGS_SUCCESS)
                return (status);
            continue;
        }

        char *short_arg = starts_with("-", arg);
        if (short_arg != NULL) {
            // Checking if this is a negative number or an option
            if (isdigit(short_arg[0]) || (short_arg[0] == '.' && isdigit(short_arg[1]))) {
                cargs_option_t *pos_opt = find_positional(options, positional_index);

                if (pos_opt && (pos_opt->value_type & VALUE_TYPE_ANY_NUMERIC)) {
                    status = handle_positional(cargs, options, arg, positional_index++);
                    if (status != CARGS_SUCCESS)
                        return (status);
                    continue;
                }
            }

            // Otherwise, handle as a regular short option
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
