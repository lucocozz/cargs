#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/internal/context.h"
#include "cargs/internal/utils.h"


cargs_error_t validate_structure(cargs_t *cargs, cargs_option_t *options);


cargs_t cargs_init(cargs_option_t *options, const char *program_name, const char *version, const char *description)
{
	cargs_t cargs = {
		.program_name = program_name,
		.version = version,
		.description = description,
		.options = options,
		.error_stack.count = 0,
	};
	context_init(&cargs);

	validate_structure(&cargs, options);
	if (cargs.error_stack.count > 0) {
		fprintf(stderr, "Error while initializing cargs:\n\n");
		cargs_print_error_stack(&cargs);
		exit(EXIT_FAILURE);
	}
	context_init(&cargs);

	return (cargs);
}
