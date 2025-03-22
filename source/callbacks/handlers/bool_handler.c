#include "cargs/errors.h"
#include "cargs/internal/utils.h"
#include "cargs/types.h"

int bool_handler(cargs_t *cargs, cargs_option_t *option, char *arg)
{
    UNUSED(cargs);
    UNUSED(arg);
    option->value.as_bool = !option->value.as_bool;
    return (CARGS_SUCCESS);
}
