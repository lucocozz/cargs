#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cargs/errors.h"
#include "cargs/internal/context.h"
#include "cargs/internal/utils.h"
#include "cargs/types.h"

int validate_structure(cargs_t *cargs, cargs_option_t *options);

cargs_t cargs_init(cargs_option_t *options, const char *program_name, const char *version)
{
    cargs_t cargs = {
        .program_name      = program_name,
        .version           = version,
        .description       = NULL,
        .env_prefix        = NULL,
        .options           = options,
        .error_stack.count = 0,
    };
    context_init(&cargs);

    if (validate_structure(&cargs, options) != CARGS_SUCCESS) {
        fprintf(stderr, "Error while initializing cargs:\n\n");
        cargs_print_error_stack(&cargs);
        exit(EXIT_FAILURE);
    }
    context_init(&cargs);

    return (cargs);
}
