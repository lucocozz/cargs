#include "cargs/errors.h"
#include "cargs/internal/display.h"
#include "cargs/internal/utils.h"
#include "cargs/types.h"

#include <stdio.h>
#include <stdlib.h>

int version_handler(cargs_t *cargs, cargs_option_t *option, char *arg)
{
    UNUSED(arg);
    UNUSED(option);
    display_version(cargs);

    return (CARGS_SUCCESS);
}
