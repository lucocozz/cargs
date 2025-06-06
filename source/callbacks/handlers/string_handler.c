#include "cargs/errors.h"
#include "cargs/internal/utils.h"
#include "cargs/types.h"

int string_handler(cargs_t *cargs, cargs_option_t *option, char *value)
{
    UNUSED(cargs);
    option->value = (cargs_value_t){.as_string = value};
    return (CARGS_SUCCESS);
}
