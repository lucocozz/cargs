#include <stdio.h>
#include <string.h>

#include "cargs/errors.h"
#include "cargs/internal/context.h"
#include "cargs/internal/parsing.h"
#include "cargs/internal/utils.h"
#include "cargs/types.h"

int handle_short_option(cargs_t *cargs, cargs_option_t *options, char *arg, char **argv, int argc,
                        int *current_index)
{
    size_t len = strlen(arg);

    // Format "-abc"
    for (size_t i = 0; i < len; ++i) {
        char            option_char = arg[i];
        cargs_option_t *option      = find_option_by_sname(options, option_char);
        if (option == NULL) {
            CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_ARGUMENT, "Unknown option: '-%c'",
                               option_char);
        }
        context_set_option(cargs, option);

        char *value = NULL;
        if (option->value_type != VALUE_TYPE_FLAG) {
            // Format "-ovalue"
            if (i < len - 1) {
                value = arg + i + 1;
                i     = len;
            } else if (*current_index + 1 < argc) {
                // Format ["-o", "value"]
                *current_index += 1;
                value = argv[*current_index];
            } else {
                CARGS_REPORT_ERROR(cargs, CARGS_ERROR_MISSING_VALUE,
                                   "Missing value for option: '-%c'", option_char);
            }
        }
        int status = execute_callbacks(cargs, option, value);
        if (status != CARGS_SUCCESS)
            return (status);
    }
    return (CARGS_SUCCESS);
}
