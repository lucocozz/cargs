#include <string.h>
#include <stdio.h>

#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/parsing.h"


int handle_short_option(cargs_t *cargs, cargs_option_t *options, char *arg, char **argv, int argc, int *current_index)
{
    size_t  len = strlen(arg);
    int     status = CARGS_SUCCESS;

    // Format "-abc"
    for (size_t i = 0; i < len; ++i)
	{
        char option_char = arg[i];        
        cargs_option_t *option = find_option_by_sname(options, option_char);
        if (option == NULL) {
            fprintf(stderr, "%s unknown: -%c\n", cargs->program_name, option_char);
            return (CARGS_ERROR_INVALID_ARGUMENT);
        }
        
        char *value = NULL;
        if (option->value_type != VALUE_TYPE_BOOL)
        {
            // Format "-ovalue"
            if (i < len - 1) {
                value = arg + i + 1;
                i = len;
            }
            else if (*current_index + 1 < argc)
            {
                // Format ["-o", "value"]
                *current_index += 1;
                value = argv[*current_index];
            }
            else
                return (CARGS_ERROR_MISSING_VALUE);
        }

        if (option->handler == NULL)
            return (CARGS_ERROR_INVALID_HANDLER);
        status = option->handler(option, value);
        if (status != CARGS_SUCCESS)
            return (status);
    }
    return (CARGS_SUCCESS);
}
