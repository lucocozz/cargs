#include "cargs/errors.h"
#include "cargs/internal/display.h"
#include "cargs/internal/utils.h"
#include "cargs/types.h"

#include <stdio.h>
#include <stdlib.h>

int help_handler(cargs_t *cargs, cargs_option_t *option, char *arg)
{
    UNUSED(arg);
    UNUSED(option);

    const cargs_option_t *command = get_active_options(cargs);
    display_usage(cargs, command);
    display_help(cargs, command);

    return (CARGS_SUCCESS);
}
