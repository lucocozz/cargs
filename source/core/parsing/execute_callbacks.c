#include "cargs/errors.h"
#include "cargs/types.h"
#include <stddef.h>
#include <stdio.h>

int execute_callbacks(cargs_t *cargs, cargs_option_t *option, char *value)
{
    int status;

    if (option->handler == NULL) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_HANDLER, "Option %s has no handler",
                           option->name);
    }

    if (option->pre_validator != NULL) {
        int status = option->pre_validator(cargs, value, option->pre_validator_data);
        if (status != CARGS_SUCCESS)
            return status;
    }

    status = option->handler(cargs, option, value);
    if (status != CARGS_SUCCESS)
        return (status);

    option->is_set = true;
    if (option->value_count == 0)
        option->value_count = 1;

    if (option->flags & FLAG_EXIT)
        return (CARGS_SOULD_EXIT);
    return (CARGS_SUCCESS);
}
