#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cargs/errors.h"
#include "cargs/internal/display.h"
#include "cargs/internal/parsing.h"
#include "cargs/internal/utils.h"
#include "cargs/types.h"

void cargs_free(cargs_t *cargs);

int cargs_parse(cargs_t *cargs, int argc, char **argv)
{
    int status = parse_args(cargs, cargs->options, argc - 1, &argv[1]);
    if (status == CARGS_SOULD_EXIT) {
        cargs_free(cargs);
        exit(CARGS_SUCCESS);
    }
    if (status != CARGS_SUCCESS) {
        printf("\n");
        display_usage(cargs, NULL);
        printf("\nTry '%s", cargs->program_name);
        for (size_t i = 0; i < cargs->context.subcommand_depth; ++i)
            printf(" %s", cargs->context.subcommand_stack[i]->name);
        printf(" --help' for more information.\n");
        return (status);
    }

    status = load_env_vars(cargs);
    if (status != CARGS_SUCCESS)
        return (status);

    status = post_parse_validation(cargs);
    return (status);
}
