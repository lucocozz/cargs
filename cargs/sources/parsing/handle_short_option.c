#include <string.h>
#include <stdio.h>

#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/utils.h"
#include "cargs/parsing.h"


int handle_short_option(cargs_t *cargs, cargs_option_t *options, char *arg, char **argv, int argc, int *current_index)
{
    size_t  len = strlen(arg);

    // Format "-abc"
    for (size_t i = 0; i < len; ++i)
	{
        char option_char = arg[i];        
        cargs_option_t *option = find_option_by_sname(options, option_char);
        if (option == NULL) {
            CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_ARGUMENT,
                "Unknown option: '-%c'", option_char);
        }
        context_set_option(cargs, option);

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
            else {
                CARGS_REPORT_ERROR(cargs, CARGS_ERROR_MISSING_VALUE,
                    "Missing value for option: '-%c'", option_char);
            }
        }

        return (execute_callbacks(cargs, option, value));
    }
    return (CARGS_SUCCESS);
}
