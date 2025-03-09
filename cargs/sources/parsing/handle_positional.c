#include "cargs/types.h"
#include "cargs/parsing.h"
#include "cargs/utils.h"

#include <stdio.h>


int handle_positional(cargs_t *cargs, cargs_option_t *options, char *value, int position) 
{
	cargs_option_t *option = find_positional(options, position);
    if (option == NULL) {
		CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_ARGUMENT,
			"Unknown positional: '%s'", value);
	}
	context_set_option(cargs, option);

	return (execute_callbacks(cargs, option, value));
}
