#include <string.h>
#include <stdio.h>

#include "cargs/utils.h"
#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/parsing.h"

int cargs_parse(cargs_t *cargs, int argc, char **argv)
{
    int status = parse_args(cargs, cargs->options, argc - 1, &argv[1]);
    if (status != CARGS_SUCCESS)
        return (status);

    status = validate_dependencies(cargs, cargs->options);
    return (status);
}
