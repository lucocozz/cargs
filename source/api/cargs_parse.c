#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "cargs/internal/utils.h"
#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/internal/parsing.h"
#include "cargs/internal/display.h"


void	cargs_free(cargs_t *cargs);

int cargs_parse(cargs_t *cargs, int argc, char **argv)
{
    int status = parse_args(cargs, cargs->options, argc - 1, &argv[1]);
    if (status == CARGS_SOULD_EXIT) {
        cargs_free(cargs);
        exit(CARGS_SUCCESS);
    }
    if (status != CARGS_SUCCESS) {
        display_usage(cargs, NULL);
        return (status);
    }

    status = validate_dependencies(cargs, cargs->options);
    return (status);
}
