#include "cargs/types.h"
#include "cargs/parsing.h"
#include "cargs/utils.h"

#include <stdio.h>


int handle_positional(cargs_t *cargs, cargs_option_t *options, char *value, int position) 
{
	cargs_option_t *option = find_positional(options, position);
    if (option == NULL) {
		fprintf(stderr, "%s: Unknown: '%s'\n", cargs->program_name, value);
        return (CARGS_ERROR_INVALID_ARGUMENT);
	}

	if (option->handler == NULL)
		return (CARGS_ERROR_INVALID_HANDLER);
	return (option->handler(cargs, option, value));
}
