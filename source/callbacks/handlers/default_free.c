#include <stdlib.h>

#include "cargs/errors.h"
#include "cargs/types.h"

int default_free(cargs_option_t *option)
{
    free(option->value.as_ptr);
    return (CARGS_SUCCESS);
}
