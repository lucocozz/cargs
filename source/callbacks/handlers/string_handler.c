#include "cargs/errors.h"
#include "cargs/internal/utils.h"
#include "cargs/types.h"

#include <stdio.h>

int string_handler(cargs_t *cargs, cargs_option_t *option, char *value)
{
    UNUSED(cargs);
    option->value = (value_t){.as_string = value};
    return (CARGS_SUCCESS);
}
