#include "cargs/errors.h"
#include "cargs/internal/utils.h"
#include "cargs/types.h"

#include <stdlib.h>

int int_handler(cargs_t *cargs, cargs_option_t *option, char *value)
{
    UNUSED(cargs);
    option->value = (value_t){.as_int64 = strtoll(value, NULL, 10)};
    return (CARGS_SUCCESS);
}
